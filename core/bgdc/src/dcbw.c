/*
 *  Copyright � 2006-2009 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  Bennu is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Bennu is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <stdlib.h>

#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include <stdio.h>
#include <string.h>

#include "bgdc.h"
#include "dcb.h"

#define SYSPROCS_ONLY_DECLARE
#include "sysprocs.h"

/* Datos externos accedidos vilmente */

extern char * string_mem;
extern int * string_offset;
extern int string_count, string_used;
extern int procdef_count;
extern int identifier_count;
extern PROCDEF ** procs;

/* Gesti�n de la lista de ficheros a incluir en el DCB */

DCB_FILE * dcb_files = 0;
int dcb_ef_alloc = 0;
int dcb_filecount = 0;
char ** dcb_fullname = 0;
int dcb_options = 0;

void dcb_add_file( const char * filename )
{
    file * fp;
    long   size;
    int i;
    char buffer[1024];

    if ( filename[0] == '@' )
    {
        fp = file_open( filename + 1, "rb" );
        if ( !fp ) return;
        while ( !file_eof( fp ) )
        {
            file_qgets( fp, buffer, sizeof( buffer ) );
            if ( strchr( buffer, '\n' ) ) *strchr( buffer, '\n' ) = 0;
            if ( buffer[0] == '#' || !buffer[0] ) continue;
            dcb_add_file( buffer );
        }
        file_close( fp );
        return;
    }

    /* No se incluyen dlls en el stub */
    if ( strlen( filename ) > 4 && !strncmp( filename + strlen( filename ) - 4, ".dll", 4 ) )   return;
    if ( strlen( filename ) > 3 && !strncmp( filename + strlen( filename ) - 3, ".so", 3 ) )    return;
    if ( strlen( filename ) > 6 && !strncmp( filename + strlen( filename ) - 6, ".dylib", 6 ) ) return;

    for ( i = 0; filename[i]; i++ ) if ( filename[i] == '\\' ) buffer[i] = '/'; else buffer[i] = filename[i];
    buffer[i] = '\0';

    filename = ( const char * ) buffer;

    fp = file_open( filename, "rb" );
    if ( !fp ) return;
    size = file_size( fp );
    file_close( fp );

    if ( dcb_ef_alloc == dcb_filecount )
    {
        dcb_files = ( DCB_FILE * )realloc( dcb_files, sizeof( DCB_FILE ) * ( dcb_ef_alloc += 16 ) );
        if ( !dcb_files ) compile_error( "dcb_add_file: out of memory" );
    }

    /* Comprueba que el fichero no haya sido ya a�adido */

    for ( i = 0; i < dcb_filecount; i++ )
        if ( strcmp( filename, (const char *)dcb_files[i].Name ) == 0 ) return;

    dcb_files[dcb_filecount].Name = (uint8_t *) strdup( filename );
    dcb_files[dcb_filecount].SFile = size;

    dcb_filecount++;
}

/* Hack para poder asignar ID's a los varspaces */

VARSPACE ** dcb_orig_varspace = 0;
int dcb_varspaces = 0;

static int dcb_varspace( VARSPACE * v )
{
    int n, result;

    for ( n = 0; n < dcb_varspaces; n++ )
        if ( dcb_orig_varspace[n] == v ) return n;

    dcb.varspace = ( DCB_VARSPACE * ) realloc( dcb.varspace, sizeof( DCB_VARSPACE ) * ( dcb_varspaces + 1 ) );
    dcb_orig_varspace = ( VARSPACE ** ) realloc( dcb_orig_varspace, sizeof( VARSPACE * ) * ( dcb_varspaces + 1 ) );

    dcb_orig_varspace[dcb_varspaces] = v;

    dcb.varspace[dcb_varspaces].NVars = v->count;   ARRANGE_DWORD( &dcb.varspace[dcb_varspaces].NVars );
    result = dcb_varspaces++;

    for ( n = 0; n < v->count; n++ )
        if ( v->vars[n].type.varspace ) dcb_varspace( v->vars[n].type.varspace );

    return result;
}

/* Conversiones de TYPEDEF */

void dcb_settype( DCB_TYPEDEF * d, TYPEDEF * t )
{
    int n;

    for ( n = 0; n < MAX_TYPECHUNKS; n++ )
    {
        d->BaseType[n] = t->chunk[n].type;          /* 8 bits */
        d->Count   [n] = t->chunk[n].count;         ARRANGE_DWORD( &d->Count[n] );
    }

    if ( t->varspace )
        d->Members = dcb_varspace( t->varspace );
    else
        d->Members = NO_MEMBERS;

    ARRANGE_DWORD( &d->Members );
}


/* Funci�n principal que crea el fichero DCB
 * (ver dcb.h con la estructura del mismo) */

DCB_HEADER dcb;

int dcb_save( const char * filename, int options, const char * stubname )
{
    file * fp;
    uint32_t n, i, size;
    long offset;
    identifier * id;
    long stubsize = 0;
    void * stubdata;

    int NPriVars = 0;
    int NPubVars = 0;

    int SCode    = 0;
    int SPrivate = 0;
    int SPublic  = 0;
    int OFilesTab = 0;

    DCB_SYSPROC_CODE sdcb;
    SYSPROC * s;
    int NSysProcs = 0;

    fp = file_open( filename, "wb0" );
    if ( !fp )
    {
        fprintf( stdout, "ERROR: can't open %s\n", filename );
        return 0;
    }

    /* Write the stub file */
    if ( stubname )
    {
        file * stub = file_open( stubname, "rb0" );

        if ( !stub )
        {
            fprintf( stdout, "ERROR: can't open %s\n", stubname );
            return 0;
        }

        stubsize = file_size( stub );
        stubdata = calloc( stubsize, sizeof( char ) );
        if ( !stubdata )
        {
            fprintf( stdout, "ERROR: Stub %s too big\n", stubname );
            return 0;
        }
        if ( file_read( stub, stubdata, stubsize ) != stubsize )
        {
            fprintf( stdout, "ERROR: error reading stub %s\n", stubname );
            return 0;
        }
        file_write( fp, stubdata, stubsize );
        if ( stubsize & 15 )
        {
            file_write( fp, stubdata, 16 - ( stubsize & 15 ) );
            stubsize += 16 - ( stubsize & 15 );
        }
        free( stubdata );
        file_close( stub );
    }

    /* Refresco lista de sysprocs */

    for ( s = sysprocs; s->name; s++ )
    {
        if ( s->id == 0 ) s->id = identifier_search_or_add( s->name );
        NSysProcs++;
    }

    /* Asumimos que dcb.varspace ha sido rellenado anteriormente */

    /* Splinter, correccion para guardar solo info necesaria */
    if ( !( options & DCB_DEBUG ) ) n_files = 0;

    /* 1. Rellenar los datos de la cabecera */

    memcpy( dcb.data.Header, DCB_MAGIC, sizeof( DCB_MAGIC ) );
    dcb.data.Version        = DCB_VERSION;                                      ARRANGE_DWORD( &dcb.data.Version );

    dcb.data.NProcs         = procdef_count;                                    ARRANGE_DWORD( &dcb.data.NProcs );
    dcb.data.NFiles         = dcb_filecount;                                    ARRANGE_DWORD( &dcb.data.NFiles );
    dcb.data.NID            = identifier_count;                                 ARRANGE_DWORD( &dcb.data.NID );
    dcb.data.NStrings       = string_count;                                     ARRANGE_DWORD( &dcb.data.NStrings );
    dcb.data.NLocVars       = local.count;                                      ARRANGE_DWORD( &dcb.data.NLocVars );
    dcb.data.NLocStrings    = local.stringvar_count;                            ARRANGE_DWORD( &dcb.data.NLocStrings );
    dcb.data.NGloVars       = global.count;                                     ARRANGE_DWORD( &dcb.data.NGloVars );

    dcb.data.SGlobal        = globaldata->current;                              ARRANGE_DWORD( &dcb.data.SGlobal );
    dcb.data.SLocal         = localdata->current;                               ARRANGE_DWORD( &dcb.data.SLocal );
    dcb.data.SText          = string_used;                                      ARRANGE_DWORD( &dcb.data.SText );

    dcb.data.NImports       = nimports;                                         ARRANGE_DWORD( &dcb.data.NImports );

    dcb.data.NSourceFiles   = n_files;                                          ARRANGE_DWORD( &dcb.data.NSourceFiles );

    dcb.data.NSysProcsCodes = NSysProcs;                                        ARRANGE_DWORD( &dcb.data.NSysProcsCodes );

    /* 2. Crear tabla de procesos */

    dcb.proc = ( DCB_PROC * ) calloc( procdef_count, sizeof( DCB_PROC ) );

    for ( n = 0; n < procdef_count; n++ )
    {
        dcb.proc[n].data.ID          = procs[n]->identifier;                    ARRANGE_DWORD( &dcb.proc[n].data.ID );
        dcb.proc[n].data.Flags       = procs[n]->flags;                         ARRANGE_DWORD( &dcb.proc[n].data.Flags );
        dcb.proc[n].data.NParams     = procs[n]->params;                        ARRANGE_DWORD( &dcb.proc[n].data.NParams );

        dcb.proc[n].data.NPriVars    = procs[n]->privars->count;                ARRANGE_DWORD( &dcb.proc[n].data.NPriVars );
        dcb.proc[n].data.NPriStrings = procs[n]->privars->stringvar_count;      ARRANGE_DWORD( &dcb.proc[n].data.NPriStrings );

        dcb.proc[n].data.NPubVars    = procs[n]->pubvars->count;                ARRANGE_DWORD( &dcb.proc[n].data.NPubVars );
        dcb.proc[n].data.NPubStrings = procs[n]->pubvars->stringvar_count;      ARRANGE_DWORD( &dcb.proc[n].data.NPubStrings );

        dcb.proc[n].data.NSentences  = procs[n]->sentence_count;                ARRANGE_DWORD( &dcb.proc[n].data.NSentences );

        dcb.proc[n].data.SPrivate    = procs[n]->pridata->current;              ARRANGE_DWORD( &dcb.proc[n].data.SPrivate );
        dcb.proc[n].data.SPublic     = procs[n]->pubdata->current;              ARRANGE_DWORD( &dcb.proc[n].data.SPublic );

        dcb.proc[n].data.SCode       = procs[n]->code.current * 4;              ARRANGE_DWORD( &dcb.proc[n].data.SCode );

        dcb.proc[n].data.OExitCode   = procs[n]->exitcode ;                     ARRANGE_DWORD( &dcb.proc[n].data.OExitCode );
        dcb.proc[n].data.OErrorCode  = procs[n]->errorcode ;                    ARRANGE_DWORD( &dcb.proc[n].data.OErrorCode );

        SCode    += procs[n]->code.current * 4;

        NPriVars += procs[n]->privars->count;
        NPubVars += procs[n]->pubvars->count;

        SPrivate += procs[n]->pridata->current;
        SPublic  += procs[n]->pubdata->current;

        dcb.proc[n].sentence = ( DCB_SENTENCE * ) calloc( procs[n]->sentence_count, sizeof( DCB_SENTENCE ) );

        for ( i = 0; i < procs[n]->sentence_count; i++ )
        {
            dcb.proc[n].sentence[i].NFile = 0;                                  ARRANGE_DWORD( &dcb.proc[n].sentence[i].NFile );
            dcb.proc[n].sentence[i].NLine = procs[n]->sentences[i].line;        ARRANGE_DWORD( &dcb.proc[n].sentence[i].NLine );
            dcb.proc[n].sentence[i].NCol  = procs[n]->sentences[i].col;         ARRANGE_DWORD( &dcb.proc[n].sentence[i].NCol );
            dcb.proc[n].sentence[i].OCode = procs[n]->sentences[i].offset;      ARRANGE_DWORD( &dcb.proc[n].sentence[i].OCode );
        }

        /* Splinter, tipos de parametros */

        dcb.proc[n].privar = ( DCB_VAR * ) calloc( procs[n]->privars->count, sizeof( DCB_VAR ) );
        for ( i = 0; i < procs[n]->privars->count; i++ )
        {
            dcb_settype( &dcb.proc[n].privar[i].Type, &procs[n]->privars->vars[i].type );

            dcb.proc[n].privar[i].ID     = procs[n]->privars->vars[i].code;     ARRANGE_DWORD( &dcb.proc[n].privar[i].ID );
            dcb.proc[n].privar[i].Offset = procs[n]->privars->vars[i].offset;   ARRANGE_DWORD( &dcb.proc[n].privar[i].Offset );
        }

        dcb.proc[n].pubvar = ( DCB_VAR * ) calloc( procs[n]->pubvars->count, sizeof( DCB_VAR ) );
        for ( i = 0; i < procs[n]->pubvars->count; i++ )
        {
            dcb_settype( &dcb.proc[n].pubvar[i].Type, &procs[n]->pubvars->vars[i].type );

            dcb.proc[n].pubvar[i].ID     = procs[n]->pubvars->vars[i].code;     ARRANGE_DWORD( &dcb.proc[n].pubvar[i].ID );
            dcb.proc[n].pubvar[i].Offset = procs[n]->pubvars->vars[i].offset;   ARRANGE_DWORD( &dcb.proc[n].pubvar[i].Offset );
        }
    }

    /* 3. Crear tablas globales */

    dcb.id = ( DCB_ID * ) calloc( identifier_count, sizeof( DCB_ID ) );

    id = identifier_first();

    for ( n = 0; n < identifier_count; n++ )
    {
        assert( id != 0 );
        strncpy( (char *)dcb.id[n].Name, id->name, sizeof( dcb.id[n].Name ) );
        dcb.id[n].Code = id->code;                                              ARRANGE_DWORD( &dcb.id[n].Code );

        id = identifier_next( id );
    }

    dcb.glovar = ( DCB_VAR * ) calloc( global.count, sizeof( DCB_VAR ) );
    dcb.locvar = ( DCB_VAR * ) calloc( local.count, sizeof( DCB_VAR ) );

    for ( n = 0; n < global.count; n++ )
    {
        dcb_settype( &dcb.glovar[n].Type, &global.vars[n].type );

        dcb.glovar[n].ID     = global.vars[n].code;                             ARRANGE_DWORD( &dcb.glovar[n].ID );
        dcb.glovar[n].Offset = global.vars[n].offset;                           ARRANGE_DWORD( &dcb.glovar[n].Offset );
    }

    for ( n = 0; n < local.count; n++ )
    {
        dcb_settype( &dcb.locvar[n].Type, &local.vars[n].type );

        dcb.locvar[n].ID     = local.vars[n].code;                              ARRANGE_DWORD( &dcb.locvar[n].ID );
        dcb.locvar[n].Offset = local.vars[n].offset;                            ARRANGE_DWORD( &dcb.locvar[n].Offset );
    }

    dcb.data.NVarSpaces = dcb_varspaces;

    dcb.file = dcb_files;

    /* 4. C�lculo de offsets */

    offset = sizeof( DCB_HEADER_DATA );

    dcb.data.OProcsTab      = offset; offset += sizeof( DCB_PROC_DATA ) * procdef_count;      ARRANGE_DWORD( &dcb.data.OProcsTab );
    dcb.data.OStrings       = offset; offset += 4 * string_count;                           ARRANGE_DWORD( &dcb.data.OStrings );
    dcb.data.OGloVars       = offset; offset += sizeof( DCB_VAR ) * global.count;             ARRANGE_DWORD( &dcb.data.OGloVars );
    dcb.data.OLocVars       = offset; offset += sizeof( DCB_VAR ) * local.count;              ARRANGE_DWORD( &dcb.data.OLocVars );
    dcb.data.OLocStrings    = offset; offset += 4 * local.stringvar_count;                  ARRANGE_DWORD( &dcb.data.OLocStrings );
    dcb.data.OID            = offset; offset += sizeof( DCB_ID ) * identifier_count;          ARRANGE_DWORD( &dcb.data.OID );
    dcb.data.OVarSpaces     = offset; offset += sizeof( DCB_VARSPACE ) * dcb_varspaces;       ARRANGE_DWORD( &dcb.data.OVarSpaces );
    dcb.data.OText          = offset; offset += string_used;                                ARRANGE_DWORD( &dcb.data.OText );
    dcb.data.OImports       = offset; offset += 4 * nimports;                               ARRANGE_DWORD( &dcb.data.OImports );
    dcb.data.OGlobal        = offset; offset += globaldata->current;                        ARRANGE_DWORD( &dcb.data.OGlobal );
    dcb.data.OLocal         = offset; offset += localdata->current;                         ARRANGE_DWORD( &dcb.data.OLocal );

    dcb.data.OSourceFiles   = offset;                                                       ARRANGE_DWORD( &dcb.data.OSourceFiles );
    /* Cada uno de los sources */
    for ( n = 0; n < n_files; n++ )
        offset += sizeof( uint32_t ) + strlen( files[n] ) + 1;

    dcb.data.OSysProcsCodes = offset;                                                       ARRANGE_DWORD( &dcb.data.OSysProcsCodes );
    for ( s = sysprocs; s->name; s++ )
    {
        offset += sizeof( DCB_SYSPROC_CODE );
        offset += s->params;
    }

    for ( n = 0; n < dcb_varspaces; n++ )
    {
        dcb.varspace[n].OVars = offset;                                                     ARRANGE_DWORD( &dcb.varspace[n].OVars );
        offset += sizeof( DCB_VAR ) * dcb_orig_varspace[n]->count;
    }

    for ( n = 0; n < procdef_count; n++ )
    {
        dcb.proc[n].data.OSentences  = offset; offset += sizeof( DCB_SENTENCE ) * procs[n]->sentence_count;       ARRANGE_DWORD( &dcb.proc[n].data.OSentences );

        /* Privadas */
        dcb.proc[n].data.OPriVars    = offset; offset += sizeof( DCB_VAR ) * procs[n]->privars->count;            ARRANGE_DWORD( &dcb.proc[n].data.OPriVars );
        dcb.proc[n].data.OPriStrings = offset; offset += 4 * procs[n]->privars->stringvar_count;                ARRANGE_DWORD( &dcb.proc[n].data.OPriStrings );
        dcb.proc[n].data.OPrivate    = offset; offset += procs[n]->pridata->current;                            ARRANGE_DWORD( &dcb.proc[n].data.OPrivate );

        /* Publicas */
        dcb.proc[n].data.OPubVars    = offset; offset += sizeof( DCB_VAR ) * procs[n]->pubvars->count;            ARRANGE_DWORD( &dcb.proc[n].data.OPubVars );
        dcb.proc[n].data.OPubStrings = offset; offset += 4 * procs[n]->pubvars->stringvar_count;                ARRANGE_DWORD( &dcb.proc[n].data.OPubStrings );
        dcb.proc[n].data.OPublic     = offset; offset += procs[n]->pubdata->current;                            ARRANGE_DWORD( &dcb.proc[n].data.OPublic );

        /* Code */
        dcb.proc[n].data.OCode       = offset; offset += procs[n]->code.current * 4;                            ARRANGE_DWORD( &dcb.proc[n].data.OCode );
    }

    /* Archivos incluidos */
    OFilesTab = offset;
    dcb.data.OFilesTab   = OFilesTab;                                                       ARRANGE_DWORD( &dcb.data.OFilesTab );

    /* FullPathName */
    dcb_fullname = calloc( dcb_filecount, sizeof( char * ) );

    /* Cada uno de los archivos incluidos */
    for ( n = 0; n < dcb_filecount; n++ )
    {
        dcb_fullname[n] = (char *)dcb.file[n].Name;                 /* Guardo el Name, porque lo destruyo en SName (es un union) */
        dcb.file[n].SName = strlen( (const char *)dcb.file[n].Name ) + 1; /* Todavia no hago el ARRANGE de este dato, lo hago luego cuando lo voy a grabar */
        offset += sizeof( DCB_FILE ) + dcb.file[n].SName;
    }

    /* Data de los archivos incluidos */
    for ( n = 0; n < dcb_filecount; n++ )
    {
        dcb.file[n].OFile = offset;                                                         ARRANGE_DWORD( &dcb.file[n].OFile );
        offset += dcb.file[n].SFile;                                                        ARRANGE_DWORD( &dcb.file[n].SFile );
    }

    /* ************************************** */
    /* ************************************** */
    /* 5. Guardar todo en disco ordenadamente */
    /* ************************************** */
    /* ************************************** */

    file_write( fp, &dcb, sizeof( DCB_HEADER_DATA ) );

    for ( n = 0; n < procdef_count; n++ )
        file_write( fp, &dcb.proc[n], sizeof( DCB_PROC_DATA ) );

    file_writeUint32A( fp, (uint32_t *)string_offset, string_count );
    file_write( fp, dcb.glovar, sizeof( DCB_VAR ) * global.count );         /* Ya procesado el byteorder */
    file_write( fp, dcb.locvar, sizeof( DCB_VAR ) * local.count );          /* Ya procesado el byteorder */
    file_writeUint32A( fp, (uint32_t *)local.stringvars, local.stringvar_count );
    file_write( fp, dcb.id, sizeof( DCB_ID ) * identifier_count );          /* Ya procesado el byteorder */
    file_write( fp, dcb.varspace, sizeof( DCB_VARSPACE ) * dcb_varspaces ); /* Ya procesado el byteorder */
    file_write( fp, string_mem, string_used );                              /* No necesita byteorder */
    file_writeUint32A( fp, (uint32_t *)imports, nimports );
    file_write( fp, globaldata->bytes, globaldata->current );   /* ****** */
    file_write( fp, localdata->bytes, localdata->current );     /* ****** */

    if ( dcb_options & DCB_DEBUG )
    {
        for ( n = 0; n < n_files; n++ )
        {
            size = strlen( files[n] ) + 1;
            file_writeUint32( fp, &size );
            file_write( fp, files[n], size );
        }
    }

    for ( s = sysprocs; s->name; s++ )
    {
        int l = s->params;
        sdcb.Id = s->id;                                                                    ARRANGE_DWORD( &sdcb.Id );
        sdcb.Type = s->type;                                                                ARRANGE_DWORD( &sdcb.Type );
        sdcb.Params = l;                                                                    ARRANGE_DWORD( &sdcb.Params );
        sdcb.Code = s->code;                                                                ARRANGE_DWORD( &sdcb.Code );
        file_write( fp, &sdcb, sizeof( DCB_SYSPROC_CODE ) );
        file_write( fp, s->paramtypes, l );
    }

    for ( n = 0; n < dcb_varspaces; n++ )
    {
        VARIABLE * var;
        DCB_VAR v;

        memset (&v, '\0', sizeof(v));

        var = &dcb_orig_varspace[n]->vars[0];

        for ( i = 0; i < dcb_orig_varspace[n]->count; i++, var++ )
        {
            dcb_settype( &v.Type, &var->type );
            v.ID     = var->code;                                                           ARRANGE_DWORD( &v.ID );
            v.Offset = var->offset;                                                         ARRANGE_DWORD( &v.Offset );

            file_write( fp, &v, sizeof( DCB_VAR ) );
        }
    }

    for ( n = 0; n < procdef_count; n++ )
    {
        file_write( fp, dcb.proc[n].sentence, sizeof( DCB_SENTENCE ) * procs[n]->sentence_count );  /* Ya procesado el byteorder */

        /* Privadas */
        file_write( fp, dcb.proc[n].privar, sizeof( DCB_VAR ) * procs[n]->privars->count );         /* Ya procesado el byteorder */
        file_writeUint32A( fp, (uint32_t *)procs[n]->privars->stringvars, procs[n]->privars->stringvar_count );
        file_write( fp, procs[n]->pridata->bytes, procs[n]->pridata->current );                     /* ****** */

        /* Publicas */
        file_write( fp, dcb.proc[n].pubvar, sizeof( DCB_VAR ) * procs[n]->pubvars->count );         /* Ya procesado el byteorder */
        file_writeUint32A( fp, (uint32_t *)procs[n]->pubvars->stringvars, procs[n]->pubvars->stringvar_count );
        file_write( fp, procs[n]->pubdata->bytes, procs[n]->pubdata->current );                     /* ****** */

        /* Code */
        file_writeUint32A( fp, (uint32_t *)procs[n]->code.data, procs[n]->code.current );
    }

    /* Cada uno de los archivos incluidos */
    for ( n = 0; n < dcb_filecount; n++ )
    {
        int siz = dcb.file[n].SName;
        ARRANGE_DWORD( &dcb.file[n].SName );
        ARRANGE_DWORD( &dcb.file[n].SFile );
        ARRANGE_DWORD( &dcb.file[n].OFile );
        file_write( fp, &dcb.file[n], sizeof( DCB_FILE ) );
        file_write( fp, dcb_fullname[n], siz );
    }

    for ( n = 0; n < dcb_filecount; n++ )
    {
        char buffer[8192];
        file * fp_r = file_open ( dcb_fullname[n], "rb" );
        int chunk_size, siz = 0;

        assert( fp_r );
        while ( !file_eof( fp_r ) )
        {
            siz += chunk_size = file_read( fp_r, buffer, 8192 );
            file_write( fp, buffer, chunk_size );
            if ( chunk_size < 8192 ) break;
        }
        file_close( fp_r );
    }

    /* Write the stub signature */

    if ( stubname != NULL )
    {
        dcb_signature dcb_signature;

        /* Voy al final del archivo */

        strcpy( dcb_signature.magic, DCB_STUB_MAGIC );
        dcb_signature.dcb_offset = ( int )stubsize;

        ARRANGE_DWORD( &dcb_signature.dcb_offset );

        file_write( fp, &dcb_signature, sizeof( dcb_signature ) );
    }

    file_close( fp );

    /* 6. Mostrar estad�sticas */

    printf( "\nFile %s compiled (%ld bytes):\n\n", filename, offset );
    printf( "  Processes              %8d\n", procdef_count );
    printf( "  Global data            %8d bytes\n", globaldata->current );
    printf( "  Local data             %8d bytes\n", localdata->current );
    printf( "  Private data           %8d bytes\n", SPrivate );
    printf( "  Public data            %8d bytes\n", SPublic );
    printf( "  Code                   %8d bytes\n", SCode );
    printf( "  System processes       %8d\n", NSysProcs );

    printf( "  Globals vars           %8d\n", global.count );
    printf( "  Locals vars            %8d\n", local.count );
    printf( "  Private vars           %8d\n", NPriVars );
    printf( "  Publics vars           %8d\n", NPubVars );
    printf( "  Identifiers            %8d\n", identifier_count );
    printf( "  Structs                %8d\n", dcb_varspaces );
    printf( "  Strings                %8d (%d bytes)\n", string_count, string_used );

    if ( dcb_filecount )
        printf( "  Files added            %8d (%ld bytes)\n", dcb_filecount, offset - OFilesTab );

    printf( "\n" );

    return 1;
}
