#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h> // for waitpid
#include <limits.h> // for PATH_MAX
#include <ncurses.h>
#include <unistd.h>
#include <ctype.h>

// small helper functions here. Big ones are after
int compareString(const void *a, const void *b) { // this will be the function used by qsort
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    return strcmp(str1, str2); // strcmp returns <0, 0, >0
}


void sanitize(char *string) {
  for (int i = 0; i < strlen(string); i++) {
    if ((!isalnum((unsigned char)string[i]) && strchr("/\\:*?\"\'<>\n\r\t", string[i])) || ((i == 0 || i == 1) && string[i] == '.')) { // replace unwanted chars by '_'. '.' is replaced if it is only the first two chars
                                                                                                                    // (TODO LATER fixe case where it "*.*", "..." and so on
        string[i] = '_';
    }
  }
}

// big critical functions here. Small ones are before
char *createNewNote(char dirToVault[PATH_MAX], char *vaultFromDir, int debug) {
  // input from user for the name
  char fileName[256];
  initscr();
  echo();
  keypad(stdscr, FALSE);
  clear();
  printw("Enter the name of the note: ");
  mvprintw(3, 0, "Unsafe charachters such as \\, /, and . will be replaced by _."); // (TODO LATER) add more info
  move(1,1);
  wgetnstr(stdscr, fileName, sizeof(fileName)-4); //limits the buffer to prevent overflow (-4 to account indexing and from ".md" in case we need to add it later)
  refresh();
  endwin();
  // (TODO LATER) add a way to go back to note selection
  if (strcmp(fileName, "") == 0) {
    printf("\e[0;32mERROR: fileName is empty\e[0m\n");
    exit(1);
  }
  // check/sanitize the input
  if (debug) {printf("\e[0;32m[DEBUG]\e[0m inputed fileName=%s\n", fileName);}
  sanitize(fileName);
  // if there is no .md add an .md
  int len = strlen(fileName);
  if (fileName[len-1] != 'd' || fileName[len-2] != 'm' || fileName[len-3] != '.') { // there might be a cleaner way to do this
    strcat(fileName, ".md"); // this should not cause an overflow issue as we get at most 252 chars (+'.'+'m'+'d'+'\0' makes it to 256) with wgetnstr
  }
  if (debug) {printf("\e[0;32m[DEBUG]\e[0m sanitize(fileName)=%s\n", fileName);} // (TODO LATER) Check if dirToVault+vaultFromDir+fileName > PATH_MAX

  char *fileFullPath = malloc(PATH_MAX); // this dinamically allocated because we use it in the main function to call openNvim
  sprintf(fileFullPath, "%s/%s/%s", dirToVault, vaultFromDir, fileName);
  FILE *filePointer;
  filePointer = fopen(fileFullPath, "w"); // creates and opens the file
  if (filePointer == NULL) {
    printf("\e[0;31mERROR: The file couldn't be created. Something went wrong.\e[0m\n");
    free(fileFullPath);
    exit(1);
  }
  fprintf(filePointer, "### %s\n", fileName); //(TODO LATER) Add a way to configure default behaviour when creating a file
  fclose(filePointer); // closes the file so that nvim could open it
  return fileFullPath;
}

char** getVaultsFromDirectory(char *dirString, size_t *count, int debug) { 
    // (TODO LATER) it might be a good idea to check if these directories exist
    // (TODO LATER) expand ~ as it does not work with opendir()
    // this function is inputed a path to a directory (which comes usually from the config file) and outpus all the suitable directories (so not the hidden ones) which will serve as separate vaults for notes
    if (debug) {printf("\e[0;32m[DEBUG]\e[0m Opening %s aka the directory of vaults\n", dirString);}

    // originally from https://www.geeksforgeeks.org/c/c-program-list-files-sub-directories-directory/
    struct dirent *vaultsDirectoryEntry;
    DIR *vaultsDirectory = opendir(dirString);
        if (vaultsDirectory == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("\e[0;31mERROR: Could not open current directory\e[0m\n" );
        exit(1); //something is fucked up
    }
    char **dirsArray = NULL; // will contain all the dirs/vaults
    size_t dirsCount = 0; // we need to count how many dirs there is to always readjust how many memory we alloc
    // Refer https://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    if (debug) {printf("┌------------------------------\n\e[0;32m[DEBUG]\e[0m Files and dirs from the directory\n");}
    while ((vaultsDirectoryEntry = readdir(vaultsDirectory)) != NULL) {
      if (debug) {printf("%s\n", vaultsDirectoryEntry->d_name);} // debugs every file/directory
      if (vaultsDirectoryEntry->d_name[0] != '.') { // if the entry don't start with a dot (so hidden dirs and hidden files)
        char fullPathEntry[PATH_MAX]; // creates a string of size of the maximum path lenght
        snprintf(fullPathEntry, sizeof(fullPathEntry), "%s/%s", dirString, vaultsDirectoryEntry->d_name); // sets the full absolute path to fullPathEntry
        
        struct stat metadataPathEntry;
        if (stat(fullPathEntry, &metadataPathEntry) == 0 && S_ISDIR(metadataPathEntry.st_mode)) { // if this entry is a directory
          dirsArray = realloc(dirsArray, (dirsCount + 1)*sizeof(char*)); // resize dirsArray so that
          dirsArray[dirsCount] = strdup(vaultsDirectoryEntry->d_name); // copy the dir name into dirsArray
          dirsCount++;
        }
      }
    }
    if (debug) {printf("└------------------------------\n");}
    // (TODO LATER) Alphabetically sort them

    // free's some used memory
    closedir(vaultsDirectory);
    *count = dirsCount;
    return dirsArray;
} 


char** getNotesFromVault(char *pathToVault, char *vault, int *count, int debug) {
    // this function is inputed a path to a vault (which was selected before) and outpus all the suitable notes (so not the hidden ones)
    if (debug) {printf("\e[0;32m[DEBUG]\e[0m Opening %s aka the vault\n", vault);}

    // originally from https://www.geeksforgeeks.org/c/c-program-list-files-sub-directories-directory/
    struct dirent *notesDirectoryEntry;
    char tempPath[PATH_MAX];
    snprintf(tempPath, sizeof(tempPath), "%s/%s", pathToVault, vault); // sets the full absolute path to fullPathEntry
    DIR *vaultDirectory = opendir(tempPath);
    if (vaultDirectory == NULL) {  // opendir returns NULL if couldn't open directory
      printf("\e[0;31mERROR: Could not open current directory\e[0m\n" );
      exit(1); //something is fucked up
    }
    char **notesArray = NULL; // will contain all the notes
    size_t notesCount = 0; // we need to count how many notes there is to always readjust how many memory we alloc
    // Refer https://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    if (debug) {printf("┌------------------------------\n\e[0;32m[DEBUG]\e[0m Files and dirs from the vault:\n");}
    while ((notesDirectoryEntry = readdir(vaultDirectory)) != NULL) {

      if (debug) {printf("%s\n", notesDirectoryEntry->d_name);}
      
      if (notesDirectoryEntry->d_name[0] != '.') { // if the entry don't start with a dot (so hidden dirs and hidden files)
        char fullPathEntry[PATH_MAX]; // creates a string of size of the maximum path lenght
        snprintf(fullPathEntry, sizeof(fullPathEntry), "%s/%s/%s", pathToVault, vault, notesDirectoryEntry->d_name); // sets the full absolute path to fullPathEntry
        
        struct stat metadataPathEntry;
        if (stat(fullPathEntry, &metadataPathEntry) == 0 && !S_ISDIR(metadataPathEntry.st_mode)) { // if this entry is a file
          notesArray = realloc(notesArray, (notesCount + 1)*sizeof(char*)); // resize notesArray so that
          notesArray[notesCount] = strdup(notesDirectoryEntry->d_name); // copy the dir name into notesArray
          notesCount++;
        }
      }
    }
    if (debug) {printf("└ ------------------------------\n");}
    // (TODO LATER) Alphabetically sort them
    // free's some used memory
    closedir(vaultDirectory);
    *count = notesCount; // passes the number of files
    return notesArray;
}

char* ncursesSelect(char **options, char *optionsType, size_t optionsNumber, int debug) { // this fonction make a TUI to select one of multiple options.
    int highlight = 0; //curently highlighted option
    int choice = 1; //selected index
    int key;

    initscr(); //initialize ncurses
    cbreak();               // disable line buffering
    noecho();               // don't echo key presses
    keypad(stdscr, TRUE);   // enable arrow keys 
    while (1) {
      clear();
      mvprintw(0,0, "Select %s (Use arrows or WASD, Enter to select):", optionsType);

            // Print options with highlighting
      for(int i = 0; i < optionsNumber; i++) {
        if (i == highlight) {
          attron(A_REVERSE);
        }   // highlight selected
        mvprintw(i+2, 2, "%s", options[i]);
        if (i == highlight) {
          attroff(A_REVERSE);
        }
      }
      key = getch(); //get some int which value correspond to some key being pressed

      switch(key) {
        case KEY_UP:
        case 'w':
        case 'W':
          highlight--;
          if (highlight < 0) {
            highlight = optionsNumber - 1;// can't select the -1nth option
          }
          break;
        case 's':
        case 'S':
        case KEY_DOWN:
          highlight++;
          if (highlight >= optionsNumber) {
            highlight = 0;
          }
          break;
        case 10: //\n
          choice = highlight;
          goto end_loop;
      }
    }
    end_loop:
    endwin(); // end ncurses mode
    return options[choice];
}


int openNvim(char *path, int debug) {
  pid_t pid = fork(); // this forking allows the programs to return when nvim is closed
  if (pid < 0) {
    perror("\e[0;31mERROR: fork failed\e[0m\n");
    return 1;
  } else if (pid == 0) {
    // Child process: replace with nvim
    if (debug) {printf("\e[0;32m[DEBUG]\e[0m Opening with command:\nnvim +:NvimTreeOpen %s\n", path);}
    execlp("nvim",
         "nvim", // we can specify each argument for nvim by passing more args to execlp
         path,
         NULL);
    perror("\e[0;31mERROR: execlp failed. Nvim might be not installed or not in path.\e[0m\n");
    exit(1);
  } else {
    // Parent process: wait for child to finish
    int status;
    waitpid(pid, &status, 0);
  }
  return 0;
}


 


int main(int argc, char *argv[]) {

    int debug = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-V") == 0) {
            debug = 0;
        } else if (strcmp(argv[i], "-v") == 0) {
            debug = 1;
        }
    }
    // Read config file
    FILE *f = fopen("./config.json", "r");
    if (!f) {
        fprintf(stderr, "\e[0;31mERROR: Could not open config.json\e[0m\n");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *data = malloc(size + 1);
    size_t read_bytes = fread(data, 1, size, f);
    if (read_bytes != size) {
        fprintf(stderr, "\e[0;31mERROR: Failed to read file\e[0m\n");
        free(data);
        fclose(f);
        return 1;
    }
    data[size] = 0;
    fclose(f);

    // Parse JSON
    cJSON *json = cJSON_Parse(data);
    if (!json) {
        fprintf(stderr, "\e[0;31mERROR: JSON parse error\[0m\n");
        free(data);
        return 1;
    }

    // Get "directory"
    cJSON *dirJsonFormat = cJSON_GetObjectItem(json, "directory");
    
    char *notesDirectoryString = NULL;
    if (dirJsonFormat && cJSON_IsString(dirJsonFormat) && dirJsonFormat->valuestring != NULL) {
        notesDirectoryString = strdup(dirJsonFormat->valuestring);  // independent string
    } else {
        notesDirectoryString = "~/Documents/Notes/";  // default
    }

    int shouldExit = 0;
    while(!shouldExit) {
      // this loop is the vault selector
      size_t vaultsCount = 0;
      char **vaultsArray = getVaultsFromDirectory(notesDirectoryString, &vaultsCount, debug);
      qsort(vaultsArray, vaultsCount, sizeof(const char *), compareString); // sorts the vaults alphabetically
      if (debug) {
        printf("┌------------------------------\n\e[0;32m[DEBUG]\e[0m Available vaults:\n");
        for (size_t i = 0; i < vaultsCount; i++) {
          printf("%s\n", vaultsArray[i]);
        }
        printf("└ ------------------------------\n");
      }
      
      // adds "create a new vault" into the vaultsArray
      const int extraOptions = 3;
      vaultsArray = realloc(vaultsArray, (vaultsCount + extraOptions)*sizeof(char*)); // resize vaultsArray to fit the extra options
      // (TODO LATER) add a way to Colorize the extraOptions
      vaultsArray[vaultsCount] = "Create a new vault"; // some more options that are not vaults
      vaultsArray[vaultsCount+1] = "Settings";
      vaultsArray[vaultsCount+2] = "Quit (Ctrl+C)";
      // select a vault
      char *vaultSelected = ncursesSelect(vaultsArray, "vault", vaultsCount + extraOptions, debug);
      if (debug) {printf("\e[0;32m[DEBUG]\e[0m Selected vault:%s\n", vaultSelected);}
      
      if (strcmp(vaultSelected,"Create a new vault") != 0 && strcmp(vaultSelected,"Settings") != 0 && strcmp(vaultSelected,"Quit (Ctrl+C)") != 0) {
        int shouldChangeVault = 0;
        while (!shouldExit && !shouldChangeVault) {
          // this loop is the note selector
          int filesCount = 0;
          char **filesArray = getNotesFromVault(notesDirectoryString, vaultSelected, &filesCount, debug);
          qsort(filesArray, filesCount, sizeof(const char *), compareString); // sorts the notes alphabetically
          
          if (debug) {
            printf("┌------------------------------\n\e[0;32m[DEBUG]\e[0m Available notes:\n");
            for (size_t i = 0; i < filesCount; i++) {
              printf("%s\n", filesArray[i]);
            }
            printf("└ ------------------------------\n");
          }
          // adds options
          int extraNotesOptions = 3;
          filesArray = realloc(filesArray, (filesCount + extraNotesOptions)*sizeof(char*)); // resize filesArray to fit the extra options
          filesArray[filesCount] = "Create new note";
          filesArray[filesCount+1] = "Back to vault selection";
          filesArray[filesCount+2] = "Quit (Ctrl+C)";
          char *noteSelected = ncursesSelect(filesArray, "note", filesCount + extraNotesOptions, debug);
          if (debug) {printf("\e[0;32m[DEBUG]\e[0m Selected note: %s\n", noteSelected);}
          
          if (strcmp(noteSelected, "Create new note") != 0 && strcmp(noteSelected,"Back to vault selection") != 0 && strcmp(noteSelected,"Quit (Ctrl+C)") != 0) {
            char fullPath[PATH_MAX];
            sprintf(fullPath, "%s/%s/%s", notesDirectoryString, vaultSelected, noteSelected);
            openNvim(fullPath, debug);
          } else if (strcmp(noteSelected,"Create new note") == 0) {
            char *pathToOpen = createNewNote(notesDirectoryString, vaultSelected, debug);
            openNvim(pathToOpen, debug);
            free(pathToOpen);
          } else if (strcmp(noteSelected,"Back to vault selection") == 0) {
            shouldChangeVault = 1;
          } else if (strcmp(noteSelected,"Quit (Ctrl+C)") == 0) {
            if (debug) {printf("\e[0;32m[DEBUG]\e[0m The program was exited,\n");}
            shouldExit = 1;
          }
        }

      } else if (strcmp(vaultSelected,"Create a new vault") == 0) {

      } else if (strcmp(vaultSelected,"Settings") == 0) {
        // (TODO LATER) add a way to modify the path to config.json
        openNvim("./config.json", debug);
      } else if (strcmp(vaultSelected,"Quit (Ctrl+C)") == 0) {
        if (debug) {printf("\e[0;32m[DEBUG]\e[0m The program was exited.\n");}
        shouldExit = 1;
      }
    }
    
    // Cleanup
    if (dirJsonFormat && notesDirectoryString != dirJsonFormat->valuestring) free(notesDirectoryString); // only free strdup
    cJSON_Delete(json);
    free(data);
    return 0;
}
