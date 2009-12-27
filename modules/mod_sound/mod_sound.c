/*
 *  Copyright © 2006-2009 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
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

#pragma comment (lib, "SDL_mixer")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "SDL_mixer.h"

#include "files.h"
#include "xstrings.h"

#include "dlvaracc.h"

/* --------------------------------------------------------------------------- */

static int audio_initialized = 0 ;

/* --------------------------------------------------------------------------- */

#define SOUND_FREQ              0
#define SOUND_MODE              1
#define SOUND_CHANNELS          2

/* --------------------------------------------------------------------------- */
/* Definicion de constantes (usada en tiempo de compilacion)                   */

DLCONSTANT  __bgdexport( mod_sound, constants_def )[] =
{
    { "MODE_MONO"   , TYPE_INT, 0  },
    { "MODE_STEREO" , TYPE_INT, 1  },
    { "ALL_SOUND"   , TYPE_INT, -1 },
    { NULL          , 0       , 0  }
} ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion)           */

char * __bgdexport( mod_sound, globals_def ) =
    "   sound_freq = 22050 ;\n"
    "   sound_mode = MODE_STEREO ;\n"
    "   sound_channels = 8 ;\n";

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                                     */
/* El interprete completa esta estructura, si la variable existe.              */
/* (usada en tiempo de ejecucion)                                              */

DLVARFIXUP  __bgdexport( mod_sound, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "sound_freq", NULL, -1, -1 },
    { "sound_mode", NULL, -1, -1 },
    { "sound_channels", NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

/* ------------------------------------- */
/* Interfaz SDL_RWops Bennu              */
/* ------------------------------------- */

static int SDLCALL __modsound_seek_cb( SDL_RWops *context, int offset, int whence )
{
    if ( file_seek( context->hidden.unknown.data1, offset, whence ) < 0 ) return ( -1 );
    return( file_pos( context->hidden.unknown.data1 ) );
}

static int SDLCALL __modsound_read_cb( SDL_RWops *context, void *ptr, int size, int maxnum )
{
    int ret = file_read( context->hidden.unknown.data1, ptr, size * maxnum );
    if ( ret > 0 ) ret /= size;
    return( ret );
}

static int SDLCALL __modsound_write_cb( SDL_RWops *context, const void *ptr, int size, int num )
{
    int ret = file_write( context->hidden.unknown.data1, ( void * )ptr, size * num );
    if ( ret > 0 ) ret /= size;
    return( ret );
}

static int SDLCALL __modsound_close_cb( SDL_RWops *context )
{
    if ( context )
    {
        file_close( context->hidden.unknown.data1 );
        SDL_FreeRW( context );
    }
    return( 0 );
}

static SDL_RWops *SDL_RWFromBGDFP( file *fp )
{
    SDL_RWops *rwops = SDL_AllocRW();
    if ( rwops != NULL )
    {
        rwops->seek = __modsound_seek_cb;
        rwops->read = __modsound_read_cb;
        rwops->write = __modsound_write_cb;
        rwops->close = __modsound_close_cb;
        rwops->hidden.unknown.data1 = fp;
    }
    return( rwops );
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : sound_init
 *
 *  Set the SDL_Mixer library
 *
 *  PARAMS:
 *      no params
 *
 *  RETURN VALUE:
 *
 *  no return
 *
 */

static int sound_init()
{
    int audio_rate;
    Uint16 audio_format;
    int audio_channels;
    int audio_buffers;
    int audio_mix_channels;

    if ( !audio_initialized )
    {
        /* Initialize variables: but limit quality to some fixed options */

        audio_rate = GLODWORD( mod_sound, SOUND_FREQ );

        if ( audio_rate > 22050 )
            audio_rate = 44100;
        else if ( audio_rate > 11025 )
            audio_rate = 22050;
        else
            audio_rate = 11025;

        audio_format = AUDIO_S16;
        audio_channels = GLODWORD( mod_sound, SOUND_MODE ) + 1;
        audio_buffers = 1024 * audio_rate / 22050;

        /* Open the audio device */
        if ( Mix_OpenAudio( audio_rate, audio_format, audio_channels, audio_buffers ) >= 0 )
        {
            GLODWORD( mod_sound, SOUND_CHANNELS ) <= 32 ? Mix_AllocateChannels( GLODWORD( mod_sound, SOUND_CHANNELS ) ) : Mix_AllocateChannels( 32 ) ;
            Mix_QuerySpec( &audio_rate, &audio_format, &audio_channels );
            audio_mix_channels = Mix_AllocateChannels( -1 ) ;
            GLODWORD( mod_sound, SOUND_CHANNELS ) = audio_mix_channels ;

            audio_initialized = 1;
            return 0;
        }
    }

    fprintf( stderr, "[SOUND] No se pudo inicializar el audio: %s\n", SDL_GetError() ) ;
    audio_initialized = 0;
    return -1 ;

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : sound_close
 *
 *  Close all the audio set
 *
 *  PARAMS:
 *      no params
 *
 *  RETURN VALUE:
 *
 *  no return
 *
 */

static void sound_close()
{
    if ( !audio_initialized ) return;

    //falta por comprobar que todo esté descargado

    Mix_CloseAudio();

    audio_initialized = 0;
}


/* ------------------ */
/* Sonido MOD y OGG   */
/* ------------------ */

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : load_song
 *
 *  Load a MOD/OGG from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *  mod pointer
 *
 */

static int load_song( const char * filename )
{
    Mix_Music *music = NULL;
    file      *fp;

    if ( !audio_initialized && sound_init() ) return ( -1 );

    fp = file_open( filename, "rb0" );
    if ( !fp )
        return ( -1 );

    music = Mix_LoadMUS_RW( SDL_RWFromBGDFP( fp ) );

    if ( music == NULL )
    {
        file_close( fp );
        fprintf( stderr, "Couldn't load %s: %s\n", filename, SDL_GetError() );
        return( -1 );
    }
    return (( int )music );

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : play_song
 *
 *  Play a MOD/OGG
 *
 *  PARAMS:
 *      mod pointer
 *      number of loops (-1 infinite loops)
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int play_song( int id, int loops )
{
    if ( !audio_initialized ) return ( -1 );

    if ((( Mix_Music * )id != NULL ) )
    {
        int result = Mix_PlayMusic(( Mix_Music * )id, loops );
        if ( result == -1 ) fprintf( stderr, "%s", Mix_GetError() );
        return result;
    }

    fprintf( stderr, "Play song called with invalid handle" );
    return( -1 );

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : fade_music_in
 *
 *  Play a MOD/OGG fading in it
 *
 *  PARAMS:
 *      mod pointer
 *      number of loops (-1 infinite loops)
 *      ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int fade_music_in( int id, int loops, int ms )
{
    if ( !audio_initialized ) return ( -1 );

    if ((( Mix_Music * )id != NULL ) )
    {
        return( Mix_FadeInMusic(( Mix_Music * )id, loops, ms ) );
    }
    return( -1 );

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : fade_music_off
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int fade_music_off( int ms )
{
    if ( !audio_initialized ) return ( -1 );
    return ( Mix_FadeOutMusic( ms ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : unload_song
 *
 *  Play a MOD
 *
 *  PARAMS:
 *
 *  mod id
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int unload_song( int id )
{
    if ( !audio_initialized ) return ( -1 );

    if (( Mix_Music * )id != NULL )
    {
        Mix_FreeMusic(( Mix_Music * )id );
        return ( 0 ) ;
    }
    return ( -1 );

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : stop_song
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int stop_song( void )
{
    if ( !audio_initialized ) return ( -1 );
    return ( Mix_HaltMusic() );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : pause_song
 *
 *  Pause the mod in curse, you can resume it after
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int pause_song( void )
{
    if ( !audio_initialized ) return ( -1 );
    Mix_PauseMusic();
    return ( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : resume_song
 *
 *  Resume the mod, paused before
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int resume_song( void )
{
    if ( !audio_initialized ) return ( -1 );
    Mix_ResumeMusic();
    return( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : is_playing_song
 *
 *  Check if there is any mod playing
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */

static int is_playing_song( void )
{
    if ( !audio_initialized ) return ( -1 );
    return Mix_PlayingMusic();
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_song_volume
 *
 *  Set the volume for mod playing (0-128)
 *
 *  PARAMS:
 *
 *  int volume
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  0 if there is no error
 *
 */

static int set_song_volume( int volume )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( volume < 0 ) volume = 0;
    if ( volume > 128 ) volume = 128;

    Mix_VolumeMusic( volume );
    return 0;
}

/* ------------ */
/* Sonido WAV   */
/* ------------ */

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : load_wav
 *
 *  Load a WAV from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *  wav pointer
 *
 */

static int load_wav( const char * filename )
{
    Mix_Chunk *music = NULL;
    file      *fp;

    if ( !audio_initialized && sound_init() ) return ( -1 );

    fp = file_open( filename, "rb0" );
    if ( !fp ) return ( -1 );

    music = Mix_LoadWAV_RW( SDL_RWFromBGDFP( fp ), 1 );
    if ( music == NULL )
    {
        fprintf( stderr, "Couldn't load %s: %s\n", filename, SDL_GetError() );
        return( -1 );
    }

    return (( int )music );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : play_wav
 *
 *  Play a WAV
 *
 *  PARAMS:
 *      wav pointer;
 *      number of loops (-1 infinite loops)
 *      channel (-1 any channel)
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  else channel where the music plays
 *
 */

static int play_wav( int id, int loops, int channel )
{
    int canal;

    if ( !audio_initialized ) return ( -1 );

    if (( Mix_Chunk * )id != NULL )
    {
        canal = Mix_PlayChannel( channel, ( Mix_Chunk * )id, loops );
        return( canal );
    }

    return ( -1 );

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : unload_wav
 *
 *  Frees the resources from a wav, unloading it
 *
 *  PARAMS:
 *
 *  wav pointer
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int unload_wav( int id )
{
    if ( !audio_initialized ) return ( -1 );

    if (( Mix_Chunk * )id != NULL )
    {
        Mix_FreeChunk(( Mix_Chunk * )id );
        return ( 0 ) ;
    }

    return ( -1 );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : stop_wav
 *
 *  Stop a wav playing
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int stop_wav( int canal )
{
    if ( !audio_initialized ) return ( -1 );

    if ( Mix_Playing( canal ) ) return( Mix_HaltChannel( canal ) );
    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : pause_wav
 *
 *  Pause a wav playing, you can resume it after
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int pause_wav( int canal )
{
    if ( !audio_initialized ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_Pause( canal );
        return ( 0 ) ;
    }
    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : resume_wav
 *
 *  Resume a wav playing, paused before
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int resume_wav( int canal )
{
    if ( !audio_initialized ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_Resume( canal );
        return ( 0 ) ;
    }

    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : is_playing_wav
 *
 *  Check a wav playing
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */

static int is_playing_wav( int canal )
{
    if ( !audio_initialized ) return ( -1 );
    return( Mix_Playing( canal ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_wav_volume
 *
 *  Set the volume for wav playing (0-128) IN SAMPLE
 *
 *  PARAMS:
 *
 *  channel id
 *  int volume
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int  set_wav_volume( int sample, int volume )
{
    if ( !audio_initialized ) return ( -1 );

    if ( volume < 0 ) volume = 0;
    if ( volume > 128 ) volume = 128;

    if (( Mix_Chunk * )sample ) return( Mix_VolumeChunk(( Mix_Chunk * )sample, volume ) );

    return -1 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_channel_volume
 *
 *  Set the volume for wav playing (0-128) IN CHANNEL
 *
 *  PARAMS:
 *
 *  channel id
 *  int volume
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int  set_channel_volume( int canal, int volume )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( volume < 0 ) volume = 0;
    if ( volume > 128 ) volume = 128;

    return( Mix_Volume( canal, volume ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : reserve_channels
 *
 *  Reserve the first channels (0 -> n-1) for the application, i.e. don't allocate
 *  them dynamically to the next sample if requested with a -1 value below.
 *
 *  PARAMS:
 *  number of channels to reserve.
 *
 *  RETURN VALUE:
 *  number of reserved channels.
 * -1 if there is any error
 *
 */

static int reserve_channels( int canales )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    return Mix_ReserveChannels( canales );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_panning
 *
 *  Set the panning for a wav channel
 *
 *  PARAMS:
 *
 *  channel
 *  left volume (0-255)
 *  right volume (0-255)
 *
 */

static int set_panning( int canal, int left, int right )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetPanning( canal, ( Uint8 )left, ( Uint8 )right );
        return ( 0 ) ;
    }

    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_position
 *
 *  Set the position of a channel. (angle) is an integer from 0 to 360
 *
 *  PARAMS:
 *
 *  channel
 *  angle (0-360)
 *  distance (0-255)
 *
 */

static int set_position( int canal, int angle, int dist )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetPosition( canal, ( Sint16 )angle, ( Uint8 )dist );
        return ( 0 ) ;
    }

    return ( -1 ) ;

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_distance
 *
 *  Set the "distance" of a channel. (distance) is an integer from 0 to 255
 *  that specifies the location of the sound in relation to the listener.
 *
 *  PARAMS:
 *
 *  channel
 *
 *  distance (0-255)
 *
 */

static int set_distance( int canal, int dist )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetDistance( canal, ( Uint8 )dist );
        return ( 0 ) ;
    }

    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : reverse_stereo
 *
 *  Causes a channel to reverse its stereo.
 *
 *  PARAMS:
 *
 *  channel
 *  flip  0 normal  != reverse
 *
 */

static int reverse_stereo( int canal, int flip )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetReverseStereo( canal, flip );
        return ( 0 ) ;
    }

    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/* Sonido                                                                      */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_load_song
 *
 *  Load a MOD from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *      mod id
 *
 */

static int modsound_load_song( INSTANCE * my, int * params )
{
    int var;

    const char * filename ;

    filename = string_get( params[0] ) ;
    if ( !filename ) return ( -1 ) ;

    var = ( load_song( filename ) );
    string_discard( params[0] );

    return ( var );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_play_song
 *
 *  Play a MOD
 *
 *  PARAMS:
 *      mod id;
 *      number of loops (-1 infinite loops)
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_play_song( INSTANCE * my, int * params )
{
    if ( params[0] == -1 ) return -1;
    return( play_song( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_unload_song
 *
 *  Frees the resources from a MOD and unloads it
 *
 *  PARAMS:
 *      mod id;
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_unload_song( INSTANCE * my, int * params )
{
    if ( params[0] < 0 ) return ( -1 );
    return( unload_song( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_stop_song
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_stop_song( INSTANCE * my, int * params )
{
    return( stop_song() );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_pause_song
 *
 *  Pause the mod in curse, you can resume it after
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_pause_song( INSTANCE * my, int * params )
{
    return( pause_song() );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_resume_song
 *
 *  Resume the mod, paused before
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_resume_song( INSTANCE * my, int * params )
{
    return( resume_song() );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_is_playing_song
 *
 *  Check if there is any mod playing
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */

static int modsound_is_playing_song( INSTANCE * my, int * params )
{
    return ( is_playing_song() );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_song_volume
 *
 *  Set the volume for mod playing (0-128)
 *
 *  PARAMS:
 *
 *  int volume
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if there is no error
 *
 */

static int modsound_set_song_volume( INSTANCE * my, int * params )
{
    return ( set_song_volume( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_fade_music_in
 *
 *  Play a MOD/OGG fading in it
 *
 *  PARAMS:
 *      mod pointer
 *      number of loops (-1 infinite loops)
 *      ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *
 */

static int modsound_fade_music_in( INSTANCE * my, int * params )
{
    if ( params[0] == -1 ) return -1;
    return ( fade_music_in( params[0], params[1], params[2] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_fade_music_off
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *
 */

static int modsound_fade_music_off( INSTANCE * my, int * params )
{
    return ( fade_music_off( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_load_wav
 *
 *  Load a WAV from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *      wav id
 *
 */

static int modsound_load_wav( INSTANCE * my, int * params )
{
    int var;

    const char * filename ;

    filename = string_get( params[0] ) ;
    if ( !filename ) return ( -1 ) ;

    var = ( load_wav( filename ) );
    string_discard( params[0] );

    return ( var );

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_play_wav
 *
 *  Play a WAV
 *
 *  PARAMS:
 *      wav id;
 *      number of loops (-1 infinite loops)
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_play_wav( INSTANCE * my, int * params )
{
    if ( params[0] == -1 ) return -1;
    return( play_wav( params[0], params[1], -1 ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_play_wav_channel
 *
 *  Play a WAV
 *
 *  PARAMS:
 *      wav id;
 *      number of loops (-1 infinite loops)
 *      channel (-1 like modsound_play_wav)
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_play_wav_channel( INSTANCE * my, int * params )
{
    if ( params[0] == -1 ) return -1;
    return( play_wav( params[0], params[1], params[2] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_unload_wav
 *
 *  Frees the resources from a wav, unloading it
 *
 *  PARAMS:
 *
 *  mod id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_unload_wav( INSTANCE * my, int * params )
{
    if ( params[0] == -1 ) return -1;
    return( unload_wav( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_stop_wav
 *
 *  Stop a wav playing
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_stop_wav( INSTANCE * my, int * params )
{
    return( stop_wav( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_pause_wav
 *
 *  Pause a wav playing, you can resume it after
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_pause_wav( INSTANCE * my, int * params )
{
    return ( pause_wav( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : resume_wav
 *
 *  Resume a wav playing, paused before
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_resume_wav( INSTANCE * my, int * params )
{
    return ( resume_wav( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : is_playing_wav
 *
 *  Check a wav playing
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */


static int modsound_is_playing_wav( INSTANCE * my, int * params )
{
    return ( is_playing_wav( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_channel_volume
 *
 *  Set the volume for a wav playing (0-128)
 *
 *  PARAMS:
 *
 *  wav id
 *  int volume
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if there is no error
 *
 */

static int modsound_set_channel_volume( INSTANCE * my, int * params )
{
    return( set_channel_volume( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_reserve_channels
 *
 *  Reserve the first channels (0 -> n-1) for the application, i.e. don't allocate
 *  them dynamically to the next sample if requested with a -1 value below.
 *
 *  PARAMS:
 *  number of channels to reserve.
 *
 *  RETURN VALUE:
 *  number of reserved channels.
 *  -1 if there is any error
 *
 */

static int modsound_reserve_channels( INSTANCE * my, int * params )
{
    return ( reserve_channels( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_wav_volume
 *
 *  Set the volume for a wav playing (0-128)
 *
 *  PARAMS:
 *
 *  wav id
 *  int volume
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if there is no error
 *
 */

static int modsound_set_wav_volume( INSTANCE * my, int * params )
{
    return( set_wav_volume( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_panning
 *
 *  Set the panning for a wav channel
 *
 *  PARAMS:
 *
 *  channel
 *  left volume (0-255)
 *  right volume (0-255)
 *
 */

static int modsound_set_panning( INSTANCE * my, int * params )
{
    return( set_panning( params[0], params[1], params[2] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_position
 *
 *  Set the position of a channel. (angle) is an integer from 0 to 360
 *
 *  PARAMS:
 *
 *  channel
 *  angle (0-360)
 *  distance (0-255)
 *
 */

static int modsound_set_position( INSTANCE * my, int * params )
{
    return( set_position( params[0], params[1], params[2] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_distance
 *
 *  Set the "distance" of a channel. (distance) is an integer from 0 to 255
 *  that specifies the location of the sound in relation to the listener.
 *
 *  PARAMS:
 *
 *  channel
 *
 *  distance (0-255)
 *
 */

static int modsound_set_distance( INSTANCE * my, int * params )
{
    return( set_distance( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_reverse_stereo
 *
 *  Causes a channel to reverse its stereo.
 *
 *  PARAMS:
 *
 *  channel
 *
 *  flip 0 normal != reverse
 *
 */

static int modsound_reverse_stereo( INSTANCE * my, int * params )
{
    return( reverse_stereo( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */

static int modsound_init( INSTANCE * my, int * params )
{
    return( sound_init() );
}

/* --------------------------------------------------------------------------- */

static int modsound_close( INSTANCE * my, int * params )
{
    sound_close();
    return( 0 );
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_sound, functions_exports )[] =
{
    { "SOUND_INIT"          , ""     , TYPE_INT , modsound_init               },
    { "SOUND_CLOSE"         , ""     , TYPE_INT , modsound_close              },
    { "LOAD_SONG"           , "S"    , TYPE_INT , modsound_load_song          },
    { "PLAY_SONG"           , "II"   , TYPE_INT , modsound_play_song          },
    { "UNLOAD_SONG"         , "I"    , TYPE_INT , modsound_unload_song        },
    { "STOP_SONG"           , ""     , TYPE_INT , modsound_stop_song          },
    { "PAUSE_SONG"          , ""     , TYPE_INT , modsound_pause_song         },
    { "RESUME_SONG"         , ""     , TYPE_INT , modsound_resume_song        },
    { "SET_SONG_VOLUME"     , "I"    , TYPE_INT , modsound_set_song_volume    },
    { "IS_PLAYING_SONG"     , ""     , TYPE_INT , modsound_is_playing_song    },
    { "LOAD_WAV"            , "S"    , TYPE_INT , modsound_load_wav           },
    { "PLAY_WAV"            , "II"   , TYPE_INT , modsound_play_wav           },
    { "UNLOAD_WAV"          , "I"    , TYPE_INT , modsound_unload_wav         },
    { "STOP_WAV"            , "I"    , TYPE_INT , modsound_stop_wav           },
    { "PAUSE_WAV"           , "I"    , TYPE_INT , modsound_pause_wav          },
    { "RESUME_WAV"          , "I"    , TYPE_INT , modsound_resume_wav         },
    { "IS_PLAYING_WAV"      , "I"    , TYPE_INT , modsound_is_playing_wav     },
    { "SET_WAV_VOLUME"      , "II"   , TYPE_INT , modsound_set_wav_volume     },
    { "FADE_MUSIC_IN"       , "III"  , TYPE_INT , modsound_fade_music_in      },
    { "FADE_MUSIC_OFF"      , "I"    , TYPE_INT , modsound_fade_music_off     },
    { "SET_CHANNEL_VOLUME"  , "II"   , TYPE_INT , modsound_set_channel_volume },
    { "RESERVE_CHANNELS"    , "I"    , TYPE_INT , modsound_reserve_channels   },
    { "SET_PANNING"         , "III"  , TYPE_INT , modsound_set_panning        },
    { "SET_POSITION"        , "III"  , TYPE_INT , modsound_set_position       },
    { "SET_DISTANCE"        , "II"   , TYPE_INT , modsound_set_distance       },
    { "REVERSE_STEREO"      , "II"   , TYPE_INT , modsound_reverse_stereo     },
    { "PLAY_WAV"            , "III"  , TYPE_INT , modsound_play_wav_channel   },
    { 0                     , 0      , 0        , 0                           }
};

/* --------------------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                               */

void  __bgdexport( mod_sound, module_initialize )()
{
    if ( !SDL_WasInit( SDL_INIT_AUDIO ) ) SDL_InitSubSystem( SDL_INIT_AUDIO );
}

/* --------------------------------------------------------------------------- */

void __bgdexport( mod_sound, module_finalize )()
{
    if ( SDL_WasInit( SDL_INIT_AUDIO ) ) SDL_QuitSubSystem( SDL_INIT_AUDIO );
}

/* --------------------------------------------------------------------------- */
