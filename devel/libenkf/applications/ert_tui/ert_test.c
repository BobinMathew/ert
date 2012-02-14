/*
   Copyright (C) 2011  Statoil ASA, Norway. 
    
   The file 'ert_test.c' is part of ERT - Ensemble based Reservoir Tool. 
    
   ERT is free software: you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by 
   the Free Software Foundation, either version 3 of the License, or 
   (at your option) any later version. 
    
   ERT is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE.   
    
   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
   for more details. 
*/

#include <enkf_fs.h>
#include <enkf_main.h>
#include <util.h>
#include <config.h>
#include <hash.h>
#include <pthread.h>
#include <enkf_types.h>
#include <string.h>
#include <local_driver.h>
#include <lsf_driver.h>
#include <signal.h>
#include <ext_joblist.h>
#include <enkf_sched.h>
#include <stringlist.h>
#include <enkf_tui_main.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <local_config.h>
#include <site_config.h>
#include <job_queue.h>
#include <ert_build_info.h>


void install_SIGNALS(void) {
  signal(SIGSEGV , util_abort_signal);    /* Segmentation violation, i.e. overwriting memory ... */
  signal(SIGINT  , util_abort_signal);    /* Control C */
  signal(SIGTERM , util_abort_signal);    /* If killing the enkf program with SIGTERM (the default kill signal) you will get a backtrace. Killing with SIGKILL (-9) will not give a backtrace.*/
}


void text_splash() {
  const int usleep_time = 1250;
  int i;
  {
#include "ERT.h"
    printf("\n\n");
    for (i = 0; i < SPLASH_LENGTH; i++) {
      printf("%s\n" , splash_text[i]);
      usleep(usleep_time);
    }
    printf("\n\n");

    sleep(1);
#undef SPLASH_LENGTH
  }
}


/*
  SVN_VERSION and COMPILE_TIME_STAMP are env variables set by the
  makefile. Will exit if the config file does not exist.
*/
void enkf_welcome(const char * config_file) {
  if (util_file_exists( config_file )) {
    char * svn_version           = util_alloc_sprintf("svn version..........: %s \n",SVN_VERSION);
    char * compile_time          = util_alloc_sprintf("Compile time.........: %s \n",COMPILE_TIME_STAMP);
    char * abs_path              = util_alloc_realpath( config_file );
    char * config_file_msg       = util_alloc_sprintf("Configuration file...: %s \n",abs_path);
    
    /* This will be printed if/when util_abort() is called on a later stage. */
    util_abort_append_version_info(svn_version);
    util_abort_append_version_info(compile_time);
    util_abort_append_version_info(config_file_msg);
    
    free(config_file_msg);
    free(abs_path);
    free(svn_version);
    free(compile_time);
  } else util_exit(" ** Sorry: can not locate configuration file: %s \n\n" , config_file);
}


void enkf_usage() {
  printf("\n");
  printf(" *********************************************************************\n");
  printf(" **                                                                 **\n");
  printf(" **                            E R T                                **\n");
  printf(" **                                                                 **\n");
  printf(" **-----------------------------------------------------------------**\n");
  printf(" ** You have sucessfully started the ert program developed at       **\n");
  printf(" ** StatoilHydro. Before you can actually start using the program,  **\n");
  printf(" ** you must create a configuration file. When the configuration    **\n");
  printf(" ** file has been created, you can start the ert application with:  **\n");
  printf(" **                                                                 **\n");
  printf(" **   bash> ert config_file                                         **\n");
  printf(" **                                                                 **\n");
  printf(" ** Instructions on how to create the configuration file can be     **\n");
  printf(" ** found at: http://sdp.statoil.no/wiki/index.php/res:enkf         **\n");
  printf(" *********************************************************************\n");
}





void kill_func( void * __enkf_main ) {
  int sleep_time = 6;
  int N          = 5;
  enkf_main_type * enkf_main = enkf_main_safe_cast( __enkf_main );
  for (int i=0; i < N; i++) {
    printf("#### %d \n",i);
    sleep( sleep_time );
  }
  printf("-----------------------------------------------------------------\n");
  {
    site_config_type * site_config = enkf_main_get_site_config( enkf_main );
    job_queue_type * job_queue = site_config_get_job_queue( site_config );

    job_queue_user_exit( job_queue );
  }
}



int main (int argc , char ** argv) {
  text_splash();
  printf("\n");
  printf("svn version : %s \n",SVN_VERSION);
  printf("compile time: %s \n",COMPILE_TIME_STAMP);
  install_SIGNALS();
  if (argc != 2) {
    enkf_usage();
    exit(1);
  } else {
    const char * site_config_file  = SITE_CONFIG_FILE;  /* The variable SITE_CONFIG_FILE should be defined on compilation ... */
    const char * model_config_file = argv[1]; 
    
    enkf_welcome( model_config_file );
    {
      enkf_main_type * enkf_main = enkf_main_bootstrap(site_config_file , model_config_file , true, true);
      printf("Bootstrap complete \n");
      /*****************************************************************/
      /* Test code of various kinds can be added here */
      {
        pthread_t kill_thread;
        pthread_create( &kill_thread , 
                        NULL ,
                        kill_func , 
                        enkf_main );
        
        enkf_tui_run_start__( enkf_main );
      }
      

      /*****************************************************************/
      enkf_main_free(enkf_main);
    }
    util_abort_free_version_info(); /* No fucking leaks ... */
  }
}
