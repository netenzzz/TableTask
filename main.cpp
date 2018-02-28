#include "application.h"

int main(int argc, char *argv[]) {
  // if( ! g_thread_supported() )
  //    g_thread_init( NULL );
  // gdk_threads_init ();
  gtk_init(&argc, &argv);
  Application app;

  return 0;
}