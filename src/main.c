/*
 Palapar, Earl Joseph
 Valerio, Jandrel Ian

 CMSC 142 Project: Unscrambler
 	A program that can generate all the k-letter words from an English dictionary
 	by unscrambling a given set of letters
*/

/* Preprocessor Directives */
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Macro Declarations */
#define TOTAL_WORDS 466544
#define WORD_LENGTH 50
#define UNDERSCORE 95
#define SUCCESS 0
#define ERROR 1

/* Function Declarations */
int loadDictionary(char **array); 
void quicksort(char **array, int lower, int higher); 
void unscramble(int n);

/* Global Variables */
char **dictionary;
char **printWords;
char scrambledWord[WORD_LENGTH] = "";
char templateQuery[WORD_LENGTH] = "";
int printWordsLen;
int scrambledWordLen;
int templateQueryLen;
int flag = 0;
int nightFlag = 0;

/* GUI Variables */
GtkWidget *window;
GtkWidget *header;
GtkWidget *query;
GtkWidget *back;
GtkWidget *toggle;
GtkWidget *stack;
GtkWidget *box1;
GtkWidget *scrambled;
GtkWidget *filter;
GtkWidget *button1;
GtkWidget *box2;
GtkWidget *label;
GtkWidget *words;

/* Main Function */
int main(int argc, char **argv) {
	int status;
	int i;

	/* 2D Array Space Allocations */
	dictionary = (char **)malloc(sizeof(char *) * TOTAL_WORDS);
	for(i = 0; i < TOTAL_WORDS; i++){
		dictionary[i] = (char *)malloc(sizeof(char) * WORD_LENGTH);
	}
	printWords = (char **)malloc(sizeof(char *) * TOTAL_WORDS);
	for(i = 0; i < TOTAL_WORDS; i++){
		printWords[i] = (char *)malloc(sizeof(char) * WORD_LENGTH);
	}

	/* Loading Dictionary */
	status = loadDictionary(dictionary);
	if (status == ERROR) return ERROR;         // Ends the application if dictionary file is not found
	quicksort(dictionary, 0, TOTAL_WORDS - 1); // Sorts the dictionary so that Binary Search is possible

	/* GUI Implementation */
	GtkBuilder *builder; 
 
	gtk_init(&argc, &argv);
 
	builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "glade/main.glade", NULL);
 
	window = GTK_WIDGET(gtk_builder_get_object(builder, "main"));
	gtk_builder_connect_signals(builder, NULL);

	header = GTK_WIDGET(gtk_builder_get_object(builder, "header"));
	query = GTK_WIDGET(gtk_builder_get_object(builder, "query"));
	back = GTK_WIDGET(gtk_builder_get_object(builder, "back"));
	toggle = GTK_WIDGET(gtk_builder_get_object(builder, "toggle"));
	stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	box1 = GTK_WIDGET(gtk_builder_get_object(builder, "box1"));
	scrambled = GTK_WIDGET(gtk_builder_get_object(builder, "scrambled"));
	filter = GTK_WIDGET(gtk_builder_get_object(builder, "filter"));
	button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
	box2 = GTK_WIDGET(gtk_builder_get_object(builder, "box2"));
	label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
	words = GTK_WIDGET(gtk_builder_get_object(builder, "words"));

	g_object_unref(builder);
	
	gtk_widget_show(window);
	printf("Application running...\n");
	gtk_main();

	/* Freeing Allocated Space */
	for(i = 0; i < TOTAL_WORDS; i++){
		free(dictionary[i]);
		free(printWords[i]);      
	}
	free(dictionary);
	free(printWords);

	printf("Application closed\n");
	return SUCCESS;
}

/* Function Definitions */
void swap(char *x, char *y) {
	char temp[WORD_LENGTH];
	strcpy(temp, x);
	strcpy(x, y);
	strcpy(y, temp);
}

void quicksort(char **array, int lower, int higher) {
	int i = lower;
	int j = higher;
	char pivot[50];
	strcpy(pivot, array[lower + (higher - lower) / 2]); // Sets median as the pivot
	while (i <= j) {
		while (strcasecmp(array[i], pivot) < 0) i++; // Finds the lower index value to swap
		while (strcasecmp(array[j], pivot) > 0) j--; // Finds the higher index value to swap
		if(i <= j){
			swap(array[i], array[j]); // Swaps values to their correct places
			i++; j--;                 // Adjust values to check next indeces
		}
	}
	/* Recursive call for partitions */
	if(lower < j) quicksort(array, lower, j);
	if(i < higher) quicksort(array, i, higher);
}

int loadDictionary(char **array) {
	char word[WORD_LENGTH] = "";
	int i;
	FILE *file = fopen("words.txt", "r");   // Opens 'words.txt' if found
	if (file != NULL) {
		for(i = 0; i < TOTAL_WORDS; i++) {
			fgets(word, WORD_LENGTH, file); // Stores each line from 'file' to 'word'
			word[strlen(word) - 1] = '\0';  // Adds '\0' to transform 'word' into a String
			strcpy(array[i], word);         // Copies 'word' into specified array index
		}
	} else {
		printf("ERROR: Dictionary file not found.\n"); // Error message
		return ERROR;                                  // Returns ERROR(1) if 'words.txt' not found
	}
	fclose(file);   // Closes 'file' for security
	return SUCCESS; // Returns SUCCESS(0) if function is sucessful
}

void unscramble(int n) {
	int nopts[n + 2];
	int option[n + 2][n + 2];
	int start, move, candidate;
	int first, last, mid;
	int i, j, k;
	char check[WORD_LENGTH] = "";
	char temp[WORD_LENGTH] = "";

	move = start = 0;
	nopts[start] = 1; // Initialization of dummy stack

	while (nopts[start] > 0) { // While dummy stack is not empty
		if (nopts[move] > 0) { // If top of stack of current position is not empty
			move++;            // Increments position
			nopts[move] = 0;   // Initialization of top of stack of current position to 0

			/* When permutation has been found */
			if (move == n + 1) {
				for (i = 1; i < move; i++) {
					check[i - 1] = scrambledWord[(option[i][nopts[i]]) - 1]; // Appends each character from 'scrambledWord' to 'check'
				}
				strncat(temp, check, templateQueryLen); // Trims 'check' to match the length of 'templateQuery'
				strcpy(check, temp);                    // Stores 'temp' back to 'check'

				first = 0;
				last = TOTAL_WORDS - 1;
				while (first <= last) {
					mid = (first + last) / 2;
					if (strcasecmp(dictionary[mid], check) == 0) {       // If word matches dictionary
						for (j = 0; j < templateQueryLen; j++) {         // Traverses 'templateQuery' characters
							if (templateQuery[j] != UNDERSCORE) {        // Checks if character is not underscore
								if (check[j] != templateQuery[j]) break; // Exits loop if character is not equal to word
							}
						}

						if (j >= templateQueryLen) {                         // If loop was not broken       
							for (k = 0; k < printWordsLen; k++) {            // Traverses 'printWords' array
								if (strcasecmp(printWords[k], check) == 0) { // Checks if word already exists in array
									break;                                   // Exits loop if it already exists
								}
							}

							if (k >= printWordsLen) {                      // If loop was not broken
							   strcpy(printWords[printWordsLen++], check); // Add word to array
							}
						}
						break; // Breaks out of the loop that searches dictionary
					}
					else {
						if (strcasecmp(dictionary[mid], check) > 0) { // If word is less than dictionary
							last = mid - 1;                           // Disregard all values at the right of mid
						}
						else {                                        // If word is greather than dictionary
							first = mid + 1;                          // Disregard all values at the left of mid
						}
					}
				}

				strcpy(check, ""); strcpy(temp, ""); // Clears the values for later use
			}

			/* Initialization of the 'option' array */
			else if (move == 1) {
				for (candidate = n; candidate >= 1; candidate--) {
					nopts[move]++;
					option[move][nopts[move]] = candidate;
				}
			}

			/* Finding Candidates */
			else {
				for (candidate = n; candidate >= 1; candidate--) {
					for (i = move - 1; i >= 1; i--) {
						if (candidate == option[i][nopts[i]]) { // Check top of stack of previous positions
							break;                              // Break if candidate is already used
						}
					}
					if (!(i >= 1)) {                             // If it did not break
						option[move][++nopts[move]] = candidate; // Add candidate to stack of options
					}
				}
			}
		}
		/* Backtracking Step */
		else {             // When candidates are exhausted
			move--;        // Move to previous position
			nopts[move]--; // Clear the current top of stack
		}
	}
}

/* Signals */
void on_button1_clicked() {
	char wordToPrint[1000] = "";
	char intbuf[50] = "";
	char lenToPrint[100] = "";
	int i;

	if (strcmp(gtk_entry_get_text(GTK_ENTRY(scrambled)), "") == 0) { // If no user input
		gtk_widget_grab_focus(scrambled);                            // Focuses on text box
	}
	else {
		strcpy(scrambledWord, gtk_entry_get_text(GTK_ENTRY(scrambled))); // Stores user input to 'scrambledWord'
		scrambledWordLen = strlen(scrambledWord);                        // Gets the length of 'scrambledWord'

		if (strcmp(gtk_entry_get_text(GTK_ENTRY(filter)), "") == 0) { // If template query is blank
			for (i = 0; i < scrambledWordLen; i++) {
				strcat(templateQuery, "_");                           // Adds underscores to 'templateQuery'
			}
			templateQueryLen = strlen(templateQuery);                 // Gets the length of 'templateQuery'
		}
		else {                                                            // If template query not blank
			strcpy(templateQuery, gtk_entry_get_text(GTK_ENTRY(filter))); // Copy user input to 'templateQuery'
			templateQueryLen = strlen(templateQuery);                     // Gets the length of 'templateQuery'
		}
		
		unscramble(scrambledWordLen);

		for(i = 0; i < printWordsLen; i++){     // Traverses 'printWords' array
			strcat(wordToPrint, printWords[i]); // Concatenates array contents to 'wordToPrint'
			strcat(wordToPrint, " ");           // Adds space to separate words
		}

		strcat(lenToPrint, "word(s) found: "); // Adds word to 'lenToPrint'
		sprintf(intbuf, "%d", printWordsLen);  // Converts int 'printWordsLen' to char 'intbuf'
		strcat(lenToPrint, intbuf);            // Adds the value of 'intbuf' to 'lenToPrint'

		gtk_label_set_text(GTK_LABEL(label), wordToPrint); // Sets corresponding label to words to print
		gtk_label_set_text(GTK_LABEL(words), lenToPrint);  // Sets corresponding label to word count
		gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), gtk_entry_get_text(GTK_ENTRY(scrambled))); // Sets subtitle to user input

		gtk_widget_hide(query); // Hide 'Add Template Query' button
		gtk_widget_show(back);  // Show 'back' button
		gtk_stack_set_visible_child (GTK_STACK(stack), box2); // Show next stack
		gtk_window_resize(GTK_WINDOW(window), 300, 100);      // Resize window to default size

		strcpy(wordToPrint, ""); // Clears values for later use
		strcpy(lenToPrint, "");  //           ""
	}
}

void on_query_clicked() {
	flag++;

	if (flag % 2 == 1) gtk_widget_show(filter);          // Shows 'Add Template Query' if hidden

	else {
		gtk_widget_hide(filter);                         // Hides 'Add Template Query' if shown
		gtk_entry_set_text(GTK_ENTRY(filter), "");       // Clears text of entry box
		gtk_window_resize(GTK_WINDOW(window), 300, 100); // Resize window
	}
}

void on_back_clicked() {
	int i;

	for (i = 0; i < printWordsLen; i++) { // Traverses 'printWords' array
		strcpy(printWords[i], "");        // Clear values for later use
	}
	printWordsLen = 0;                    // Set length to 0

	gtk_widget_hide(back);
	gtk_widget_show(query);
	gtk_label_set_text(GTK_LABEL(label), "");
	gtk_label_set_text(GTK_LABEL(words), ""); 
	strcpy(templateQuery, "");
	strcpy(scrambledWord, "");
	gtk_window_resize(GTK_WINDOW(window), 300, 100);         // Resize window
	gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), ""); // Clears value of subtitle
	gtk_stack_set_visible_child (GTK_STACK(stack), box1);    // Shows previous stack
}

void on_toggle_clicked() {
	nightFlag++;

	if (nightFlag % 2 == 1) {
		g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
		gtk_widget_set_tooltip_text(toggle, "Light Theme");
	}

	else {
		g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", FALSE, NULL);
		gtk_widget_set_tooltip_text(toggle, "Dark Theme");
	}
}

/* When close button was clicked */
void on_main_destroy() {
	gtk_main_quit();
}