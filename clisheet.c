
// test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h> // usleep


#include <dirent.h> //path_max
#include <sys/stat.h>
#include <time.h> 
#include <math.h>



int rows, cols; 
char filesource[PATH_MAX];
char linefilter[PATH_MAX];
char linefiltertime[PATH_MAX];
char fullcurrentline[PATH_MAX];
int user_selection = 1;
int user_celly = 1;
int user_cellx = 1;
int user_scrolly = 0;
int user_scrollx = 0;
int user_autorefresh = 0;
int app_tabspace = 8;
int app_colspace = 0;
int user_show_line = 0;
int user_show_sum = 0;
int user_show_popupsum = 0;
int cellx_start = 1;
float cellsum = 0; 
char statusbar_cellcontent[PATH_MAX];
char fullcellfize[PATH_MAX];




/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
char *strrlf(char *str)
{  
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if (str[i] != '\n' && str[i] != '\n') 
        ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}




char *strcut( char *str , int myposstart, int myposend )
{  
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( ( str[i] != '\0' ) && ( str[i] != '\0') )
         if ( ( i >=  myposstart-1 ) && (  i <= myposend-1 ) )
           ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}


///////////////////////////////
///////////////////////////////
///////////////////////////////


///////////////////////////////
///////////////////////////////
///////////////////////////////
char *strtimestamp()
{
      long t;
      struct tm *ltime;
      time(&t);
      ltime=localtime(&t);
      char charo[50];  int fooi ; 
      fooi = snprintf( charo, 50 , "%04d%02d%02d%02d%02d%02d",
	1900 + ltime->tm_year, ltime->tm_mon +1 , ltime->tm_mday, 
	ltime->tm_hour, ltime->tm_min, ltime->tm_sec 
	);
    size_t siz = sizeof charo ; 
    char *r = malloc( sizeof charo );
    return r ? memcpy(r, charo, siz ) : NULL;
}


char *strtrim(char *str)
{  
      // right side to to finish
      char ptr[strlen(str)+1];
      int strposmax = strlen( str );
      int lastposchar = strposmax;
      int i,j=0;
      int foundspace = 1;

      /// find last char
      foundspace = 1;
      for( i= strposmax-1 ; i >= 0 ; i--)
      {
         //printf( "|%d-%d-%c| ", i , lastposchar , str[i] );
	 // find where to space
         if ( foundspace == 1 ) 
         if ( str[i] == ' ' ) 
   	    lastposchar = i-1;

         if ( str[i] != ' ' ) 
           foundspace = 0;
      } 

      // add the content
      foundspace = 1;
      for(i=0; i <= lastposchar ; i++)
      {
        if ( foundspace == 1 ) 
         if ( ( str[i] != ' ' )  && ( str[i] != 9 ) ) //added! with 9 for a TAB!!
          foundspace = 0;

        if ( foundspace == 0 ) 
           ptr[j++]=str[i];
      } 
      ptr[j]='\0';

      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}






////////////////////////////////
void ncurses_runcmd( char *thecmd   )
{
       char rncmdi[PATH_MAX];
       def_prog_mode();
       endwin();
       strncpy( rncmdi , "  " , PATH_MAX );
       strncat( rncmdi , thecmd , PATH_MAX - strlen( rncmdi ) -1 );
       strncat( rncmdi , " " , PATH_MAX - strlen( rncmdi ) -1 );
       system( rncmdi );
       reset_prog_mode();
}



////////////////////////////////
void ncurses_runwith( char *thecmd , char *thestrfile  ) //thecmd first
{
       char cmdi[PATH_MAX];
       def_prog_mode();
       endwin();
       printf( "<NDESK CMD: Command...>\n" );
       strncpy( cmdi , "  " , PATH_MAX );
       strncat( cmdi , thecmd , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , thestrfile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\" " , PATH_MAX - strlen( cmdi ) -1 );
       printf( "<NDESk CMD: %s>\n", cmdi );
       system( cmdi );
       reset_prog_mode();
}





/////////////////////////////////
/////////////////////////////////
/////////////////////////////////
int fexist(char *a_option)
{
  char dir1[PATH_MAX]; 
  char *dir2;
  DIR *dip;
  strncpy( dir1 , "",  PATH_MAX  );
  strncpy( dir1 , a_option,  PATH_MAX  );

  struct stat st_buf; 
  int status; 
  int fileordir = 0 ; 

  status = stat ( dir1 , &st_buf);
  if (status != 0) {
    fileordir = 0;
  }

  // this is compatible to check if a file exists
  FILE *fp2check = fopen( dir1  ,"r");
  if( fp2check ) {
  // exists
  fileordir = 1; 
  fclose(fp2check);
  } 

  if (S_ISDIR (st_buf.st_mode)) {
    fileordir = 2; 
  }
return fileordir;
/////////////////////////////
}







//////////////////
void ncp( char *filetarget,  char *  filesource )
{
  // fread
  char            buffer[1];
  size_t          n;
  size_t          m;
  FILE *fp;
  FILE *fp1; 
  FILE *fp2;
  int counter = 0 ; 
  int freader = 1 ; 
  int i , j ,posy, posx ; 
  if ( fexist( filesource ) == 1 )
  {
        fp = fopen( filesource, "rb");
        fp2 = fopen( filetarget, "wb");
        counter = 0; 
        while(  !feof(fp) && ( freader == 1)   ) {
           if (  feof(fp)   ) {
                freader = 0 ; 
            }
            n = fread(  buffer, sizeof(char), 1 , fp);
            m = fwrite( buffer, sizeof(char), 1,  fp2);
        }
        fclose(fp2);
        fclose(fp);
      }
}




////////////////////////
////////////////////////
////////////////////////
////////////////////////
char *strninput( char *myinitstring )
{

   int strninput_gameover = 0; 
   char strmsg[PATH_MAX];
   char charo[PATH_MAX];
   strncpy( strmsg, myinitstring , PATH_MAX );

   int ch ;  int foo ; 
   int fooj; 
   while ( strninput_gameover == 0 )
   {
                  getmaxyx( stdscr, rows, cols);
                  attroff( A_REVERSE );
                  for ( fooj = 0 ; fooj <= cols-1;  fooj++)
                  {
                    mvaddch( rows-1, fooj , ' ' );
                  }
                  mvprintw( rows-1, 0, ":> %s" , strrlf( strmsg ) );
                  attron( A_REVERSE );
                  printw( " " );
                  attroff( A_REVERSE );
                  attroff( A_REVERSE );
                  attroff( A_BOLD );
                  refresh() ; 

                  curs_set( 0 );
                  ch = getch();

  if ( ch == 8 ) 
    strncpy( strmsg, strcut( strmsg, 1 , strlen( strmsg ) -1 )  ,  PATH_MAX );

   else if ( ch == 20 ) strncpy( strmsg, strtimestamp() ,  PATH_MAX );  //ctrl+t


   else if ( ch == 4 ) 
   {
     strncat( strmsg ,  strtimestamp()  , PATH_MAX - strlen( strmsg ) -1 );  //ctrl+d for dateit
   }

  else if ( ch == 2 ) strncpy( strmsg,  ""  ,  PATH_MAX );  // bsd

  else if ( ch == 27 )  
  {
    strncpy( strmsg, ""  ,  PATH_MAX );
    strninput_gameover = 1;
  }

  else if ( ch == 274 )  
  {
    strncpy( strmsg, ""  ,  PATH_MAX );
    strninput_gameover = 1;
  }

  else if ( ch == 263 )  
    strncpy( strmsg, strcut( strmsg, 1 , strlen( strmsg ) -1 )  ,  PATH_MAX );



		  else if ( ch == 4 ) 
		  {
			   strncpy( strmsg, ""  ,  PATH_MAX );
		  }
		  else if ( ch == 27 ) 
		  {
			   strncpy( strmsg, ""  ,  PATH_MAX );
		  }
	          else if (
			(( ch >= 'a' ) && ( ch <= 'z' ) ) 
		        || (( ch >= 'A' ) && ( ch <= 'Z' ) ) 
		        || (( ch >= '1' ) && ( ch <= '9' ) ) 
		        || (( ch == '0' ) ) 
		        || (( ch == '~' ) ) 
		        || (( ch == '!' ) ) 
		        || (( ch == '&' ) ) 
		        || (( ch == '=' ) ) 
		        || (( ch == ':' ) ) 
		        || (( ch == ';' ) ) 
		        || (( ch == '<' ) ) 
		        || (( ch == '>' ) ) 
		        || (( ch == ' ' ) ) 
		        || (( ch == '|' ) ) 
		        || (( ch == '#' ) ) 
		        || (( ch == '?' ) ) 
		        || (( ch == '+' ) ) 
		        || (( ch == '/' ) ) 
		        || (( ch == '\\' ) ) 
		        || (( ch == '.' ) ) 
		        || (( ch == '$' ) ) 
		        || (( ch == '%' ) ) 
		        || (( ch == '-' ) ) 
		        || (( ch == ',' ) ) 
		        || (( ch == '{' ) ) 
		        || (( ch == '}' ) ) 
		        || (( ch == '(' ) ) 
		        || (( ch == ')' ) ) 
		        || (( ch == ']' ) ) 
		        || (( ch == '[' ) ) 
		        || (( ch == '*' ) ) 
		        || (( ch == '"' ) ) 
		        || (( ch == '@' ) ) 
		        || (( ch == '-' ) ) 
		        || (( ch == '_' ) ) 
		        || (( ch == '^' ) ) 
		        || (( ch == '\'' ) ) 
	             ) 
		  {
                           foo = snprintf( charo, PATH_MAX , "%s%c",  strmsg, ch );
			   strncpy( strmsg,  charo ,  PATH_MAX );
		  }
		  else if ( ch == 10 ) 
		  {
                        strninput_gameover = 1;
		  }
     }
     char ptr[PATH_MAX];
     strncpy( ptr, strmsg, PATH_MAX );
     size_t siz = sizeof ptr ; 
     char *r = malloc( sizeof ptr );
     return r ? memcpy(r, ptr, siz ) : NULL;
}






void gfxhline( int yyline )
{
    int y1 = 0; int x1=0; 
    int y2 = rows-1; int x2= cols -1 ;
    int foox ; int fooy; 
    //color_set( 7, NULL ); attron( A_REVERSE ); 
    //for( fooy = y1 ; fooy <= y2 ; fooy++) 
    fooy = yyline ; 
    for( foox = x1 ; foox <= x2 ; foox++) 
      mvprintw( fooy , foox , " " );
}


void void_cls()
{
    int y1 = 0; int x1=0; 
    int y2 = rows-1; int x2= cols -1 ;
    int foox ; int fooy; 
    color_set( 7, NULL ); attron( A_REVERSE ); 
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
    for( foox = x1 ; foox <= x2 ; foox++) 
      mvprintw( fooy , foox , " " );
}


void gfxframe( int y1, int x1, int y2, int x2 )
{
    int foo, fooy , foox ;
    foo = x1;
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
        mvaddch( fooy , foo , ACS_VLINE );
    foo = x2;
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
         mvaddch( fooy , foo , ACS_VLINE );
    foo = y1;
    for( foox = x1 ; foox <= x2 ; foox++) 
         mvaddch( foo , foox , ACS_HLINE );
    foo = y2;
    for( foox = x1 ; foox <= x2 ; foox++) 
         mvaddch( foo , foox , ACS_HLINE );
    mvaddch( y1 , x1 , ACS_ULCORNER );
    mvaddch( y1 , x2 , ACS_URCORNER );
    mvaddch( y2 , x1 , ACS_LLCORNER );
    mvaddch( y2 , x2 , ACS_LRCORNER );
}


void mvcenter( int myposypass, char *mytext )
{
      mvprintw( myposypass, cols/2 - strlen( mytext )/2  , "%s", mytext );
}





////////////////////////
void void_draw()
{
   int y1 = 0; int x1=0; 
   int y2 = rows-1; int x2= cols -1 ;
   int foox ; int fooy; 
   color_set( 7, NULL ); attron( A_REVERSE ); 
   for( fooy = y1 ; fooy <= y2 ; fooy++) 
   for( foox = x1 ; foox <= x2 ; foox++) 
      mvprintw( fooy , foox , " " );
    
  int foocelly = 1;
  int foocellx = 1;

  int posy; int posx;
  posy = 2; 
  char charo[PATH_MAX];
  strncpy( statusbar_cellcontent , "" , PATH_MAX );

  int foundline = 1; 
  int fetchj;
  FILE *fp5;
  FILE *fp6;
  int fetchi; char fetchline[PATH_MAX]; char fetchlinetmp[PATH_MAX];

  int linepos = 0;  int poscount ; 
  strncpy( fullcurrentline , "" , PATH_MAX );
  cellsum = 0; 
  /////////////////////////////////////////////////
  if ( fexist( filesource ) == 1 )
  {
     fp6 = fopen( filesource , "rb");
     while( !feof(fp6) ) 
     {
          foundline = 1; 
          foocellx = 1;
          fgets(fetchlinetmp, PATH_MAX, fp6); 
          strncpy( fullcellfize , "" , PATH_MAX );

          strncpy( fetchline, "" , PATH_MAX );
          for( fetchi = 0 ; ( fetchi <= strlen( fetchlinetmp ) ); fetchi++ )
            if ( fetchlinetmp[ fetchi ] != '\n' )
              fetchline[fetchi]=fetchlinetmp[fetchi];

         if ( strcmp( linefilter, "" ) == 0 ) 
         {
              foundline = 1; 
         }
         else 
         {
              if ( strstr( fetchline, linefilter ) != 0 ) 
                foundline = 1; 
              else
                foundline = 0; 
         }

         if ( ( foundline == 1 ) && ( strcmp( linefiltertime, "" ) == 0 ) )
         {
              foundline = 1; 
         }
         else if ( foundline == 1 ) 
         {
              if ( strstr( fetchline, linefiltertime ) != 0 ) 
                foundline = 1; 
              else
                foundline = 0; 
         }


         if ( foundline == 1 ) 
             linepos++;


         if ( !feof(fp6) )
         if ( foundline == 1 ) 
         if ( linepos >= user_scrolly ) 
         {
             foocellx = 1;
             poscount = 0; 
             posx     = 2;
             attron( A_REVERSE );
             if ( posy <= rows - 3) 
             {
               if ( foocelly == user_celly ) 
                  mvprintw( posy, 1, ">" );

               mvprintw( posy, 2, "" );
               for( fetchi = 0 ; ( fetchi <= strlen( fetchline ) ); fetchi++ )
               {
                 if ( foocelly == user_celly ) 
                 {
                    snprintf( charo, PATH_MAX , "%c", fetchline[ fetchi ] );
                    strncat( fullcurrentline , charo , PATH_MAX - strlen( fullcurrentline ) -1 );
                 }

                 if ( ( user_show_sum == 1 ) || ( user_show_popupsum == 1 ) )
                 if ( foocellx == 5 ) // de nbr 
                 {
                     if ( fetchline[ fetchi ] == '.' )
                     {  }
                     else if ( fetchline[ fetchi ] == ',' )
                     {
                        strncat( fullcellfize , "." , PATH_MAX - strlen( fullcellfize ) -1 );
                     }
                     else
                     {    snprintf( charo, PATH_MAX , "%c", fetchline[ fetchi ] ); 
                          strncat( fullcellfize , charo , PATH_MAX - strlen( fullcellfize ) -1 );
                     }
                 }


                 if ( fetchline[ fetchi ] == '\t' )
                 {
                    if (foocellx >= cellx_start )
                    {
                       for( fetchj = 0 ; ( fetchj <= app_tabspace ); fetchj++ )
                         addch( ' ' );

                       posx = posx + app_tabspace;
                       poscount = 0; 
                    }
                    foocellx++;
                 }
                 else if ( fetchline[ fetchi ] == ';' )
                 {
                    if (foocellx >= cellx_start )
                    {
                      for( fetchj = 0 ; ( fetchj <= app_tabspace ); fetchj++ )
                        addch( ' ' );

                      poscount = 0; 
                      posx = posx + app_tabspace;
                    }
                    foocellx++;
                 }
                 else
                 {
                     attron( A_REVERSE );

                     if ( ( foocelly == user_celly ) && ( foocellx == user_cellx ) )
                     {
                        attroff( A_REVERSE );
                        snprintf( charo, PATH_MAX , "%c", fetchline[ fetchi ] );
                        strncat( statusbar_cellcontent , charo , PATH_MAX - strlen( statusbar_cellcontent ) -1 );
                     }

                     {
                        if ( app_colspace == 0 )  
                        {
                           if ( posx <= cols-2 -5)
                           {
                            printw( "%c", fetchline[fetchi] );
                            posx = posx + 1;
                            poscount++;
                           }
                        }

                        else if ( app_colspace != 0 )  
                        {
                            if ( foocellx >= cellx_start ) 
                            if ( poscount <= app_colspace )
                            if ( posx <= cols-2 -5)
                            {
                              printw( "%c", fetchline[fetchi] );
                              posx = posx + 1;
                              poscount++;
                            }
                        }
                        //poscount++;
                     }

                 }
               }

               if ( atof( fullcellfize ) !=  0 )
                   cellsum = cellsum + atof( fullcellfize );
             
               foocelly++;
               posy++;


             }
 
	 }
     }
     fclose( fp6 );
     }


    attron( A_REVERSE );
    gfxframe( 1, 0 , rows-3, cols-1 );
    attroff( A_REVERSE );
    gfxhline( 0 );
    mvprintw( 0 , 0, "|CLISHEET|");
    attroff( A_REVERSE );
    gfxhline( rows-2 );
    mvprintw( rows-2 , 0, "[%d,%d][%d][LINE:%s]", user_cellx, user_celly, user_scrolly , statusbar_cellcontent ); 
    gfxhline( rows-1 );
    mvprintw( rows-1 , 0, "FILE: %s (%d)", filesource, fexist( filesource ) );

    long t;
    struct tm *ltime;
    time(&t);
    ltime=localtime(&t);
    snprintf( charo, 50 , "%04d/%02d/%02d %02d:%02d:%02d",
	1900 + ltime->tm_year, ltime->tm_mon +1 , ltime->tm_mday, 
	ltime->tm_hour, ltime->tm_min, ltime->tm_sec );
    mvprintw( rows-1 , cols-1-20+2 , "%s", charo ); 


    if ( user_autorefresh == 1 ) 
    {
          mvprintw( rows-2, cols-1 -14 , "[AUTOREFRESH]" );
    }
}





int main( int argc, char *argv[])
{

  initscr();	
  getmaxyx( stdscr, rows, cols);
  curs_set( 0 );
  noecho();            
  keypad( stdscr, TRUE );  // for F...
  start_color();
  init_pair(0,  COLOR_WHITE,     COLOR_BLACK);
  init_pair(1,  COLOR_RED,     COLOR_BLACK);
  init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
  init_pair(3,  COLOR_YELLOW,  COLOR_BLACK);
  init_pair(4,  COLOR_BLUE,    COLOR_BLACK);
  init_pair(5,  COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6,  COLOR_CYAN,    COLOR_BLACK);
  init_pair(7,  COLOR_BLUE,    COLOR_WHITE);

  int foo, fooy , foox ;

     ////////////////////////////////////////////////////////
     //if ( strcmp( argv[2] , "all" ) ==  0 ) 
     if ( argc == 2)
          strncpy( filesource, argv[ 1 ] ,  PATH_MAX );
      else
          strncpy( filesource, "file.csv" , PATH_MAX );


    char fooline[PATH_MAX];
    strncpy( linefilter, "" , PATH_MAX );
    strncpy( linefiltertime, "" , PATH_MAX );

    void_cls(); 

    color_set( 7, NULL ); attron( A_REVERSE ); 
    gfxframe( 0, 0 , rows-1, cols -1 );
    attroff( A_BOLD ); 
    //mvcenter( rows*40/100 -1, "[ WELCOME - Press Key ]" );
    

    int ch ;  
    int gameover = 0; 
    while ( gameover == 0 ) 
    {
        if ( cellx_start <= 1 )  cellx_start = 1; 
        if ( user_scrolly <= 0 ) user_scrolly = 0; 
        if ( user_scrollx <= 0 ) user_scrollx = 0; 
        if ( user_cellx <= 0 ) user_cellx = 0; 
        if ( user_celly <= 0 ) user_celly = 0; 

        getmaxyx( stdscr, rows, cols);
        void_draw();

        attroff( A_REVERSE );
        if ( user_show_line == 1 )
        {
              gfxhline( rows/2 );
              gfxhline( rows/2-1 );
              gfxhline( rows/2-2 );
              gfxhline( rows/2-3 );
              gfxhline( rows/2+1 );
              gfxhline( rows/2+2 );
              gfxhline( rows/2+3 );
              mvprintw( rows/2, 0, "|%s|", fullcurrentline);
        }

        //loc
        mvprintw( rows-3, 2, "" ); 
        if ( user_show_sum == 1 )
           printw( "|2.Sum: %f|", cellsum );
        // last
        if ( user_show_popupsum == 1 )
        {
           printw( "|3.Sum: %f|", cellsum );
           gfxhline( rows/2+6 );
           mvprintw( rows/2+6, cols/2-10, "|3.Sum: %f|", cellsum );
        }


        if ( user_autorefresh == 1 ) 
        {
          nodelay( stdscr, TRUE);
          curs_set( 0 );
          usleep( 1e5 );
        }
        else
          nodelay( stdscr, FALSE);

        curs_set( 0 );
        ch = getch();
        curs_set( 1 );

        switch( ch ) 
        {
           case 'q':
           case KEY_F(10):
              gameover = 1;
              break;

           case 'm':
              ch = getch();
              if ( ch == 'c' )
               ncurses_runcmd( " bash ");
              break;

           case 'z':
           case '1':
              if (  user_show_line == 0 ) 
                  user_show_line = 1; 
              else
                  user_show_line = 0; 
              break;

           case '2':
              if (  user_show_sum == 0 ) user_show_sum = 1; else user_show_sum = 0; 
              break;
           case '3':
              if (  user_show_popupsum == 0 ) user_show_popupsum = 1; else user_show_popupsum = 0; 
              break;

           case 'F':
           case KEY_F(7):
              strncpy( fooline, strninput( linefiltertime ) , PATH_MAX );
              strncpy( linefiltertime, fooline , PATH_MAX );
              break;

           case 'f':
           case KEY_F(3):
              strncpy( fooline, strninput( linefilter ) , PATH_MAX );
              if ( strcmp( fooline, linefilter ) != 0 )
              {
                 user_cellx = 1; 
                 user_celly = 1; 
                 user_scrolly = 0; 
                 user_scrollx = 0; 
              }
              strncpy( linefilter, fooline , PATH_MAX );
              break;

           case '!':
              strncpy( fooline, strninput( "" ) , PATH_MAX );
              if ( strcmp( fooline, "" ) != 0 )
                 ncurses_runwith(  fooline , filesource );
              break;

           case '$':
              strncpy( fooline, strninput( "" ) , PATH_MAX );
              if ( strcmp( fooline, "" ) != 0 )
                 ncurses_runcmd(  fooline );
              break;

           case KEY_F(4):
              ncurses_runwith( " fledit " , filesource );
              break;

           case KEY_RIGHT:
           case 'l':
              user_cellx++;
              break;
           case KEY_LEFT:
           case 'h':
              user_cellx--;
              break;

           case 'j':
           case KEY_DOWN:
              user_celly++;
              break;
           case KEY_UP:
           case 'k':
              user_celly--;
              break;

           case '0':
              user_cellx = 1; 
              break;

           case 'G':
              user_cellx = 1;
              user_celly = 1; 
              user_scrolly = 0; 
              user_scrollx = 0; 
              break;
           case 'g':
              user_cellx = cellx_start; 
              user_celly = 1; 
              user_scrolly = 0; 
              user_scrollx = 0; 
              break;

           case 32:
           case 'd':
              user_scrolly++;
              user_scrolly++;
              user_scrolly++;
              user_scrolly++;
              break;
           case 'u':
              user_scrolly--;
              user_scrolly--;
              user_scrolly--;
              user_scrolly--;
              break;

           case 'b':
              user_scrolly--;
              break;
           case 'n':
              user_scrolly++;
              break;

           case '(':
              if ( app_colspace >= 1 ) 
              app_colspace--;
              break;
           case ')':
              app_colspace++;
              break;

           case '[':
              if ( cellx_start >= 1 ) 
              cellx_start--;
              break;
           case ']':
              cellx_start++;
              break;

           case '<':
              if ( app_tabspace >= 1 ) 
              app_tabspace--;
              break;
           case '>':
              app_tabspace++;
              break;

           case '#':
              if ( user_autorefresh == 1 ) 
                  user_autorefresh = 0; 
                  else 
                  user_autorefresh = 1;
              break;

           case 'v':
              ncurses_runwith( " vim " , filesource );
              break;
        }
    }



   curs_set( 4 ) ;
   attroff( A_BOLD ); attroff( A_REVERSE ); curs_set( 1 );
   endwin();		/* End curses mode  */
   return 0;
}



