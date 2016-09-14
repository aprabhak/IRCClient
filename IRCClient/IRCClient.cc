
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
//#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

GtkListStore * list_rooms;
GtkListStore * list_users;

//char * user;
//char * password;
char * host;
char * sport;
int port;
//int i = 0;
GtkTextBuffer *globalbuffer;

int open_client_socket(char * host, int port) {
  // Initialize socket address structure
  struct  sockaddr_in socketAddress;
  
  // Clear sockaddr structure
  memset((char *)&socketAddress,0,sizeof(socketAddress));
  
  // Set family to Internet 
  socketAddress.sin_family = AF_INET;
  
  // Set port
  socketAddress.sin_port = htons((u_short)port);
  
  // Get host table entry for this host
  struct  hostent  *ptrh = gethostbyname(host);
  if ( ptrh == NULL ) {
    perror("gethostbyname");
    exit(1);
  }

  memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

  struct  protoent *ptrp = getprotobyname("tcp");
  if ( ptrp == NULL ) {
    perror("getprotobyname");
    exit(1);
  }
  
  // Create a tcp socket
  int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }
  
  // Connect the socket to the specified server
  if (connect(sock, (struct sockaddr *)&socketAddress,
        sizeof(socketAddress)) < 0) {
    perror("connect");
    exit(1);
  }
  
  return sock;
}

#define MAX_RESPONSE (10 * 1024)
int sendCommand(char * host, int port, char * command, char * user,
    char * password, char * args, char * response) {
  int sock = open_client_socket( host, port);

  // Send command
  write(sock, command, strlen(command));
  write(sock, " ", 1);
  write(sock, user, strlen(user));
  write(sock, " ", 1);
  write(sock, password, strlen(password));
  write(sock, " ", 1);
  write(sock, args, strlen(args));
  write(sock, "\r\n",2);

  // Keep reading until connection is closed or MAX_REPONSE
  int n = 0;
  int len = 0;
  while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
    len += n;
  }

  //printf("response:%s\n", response);

  close(sock);
}

void update_list_rooms() {
    GtkTreeIter iter;
    int i;

    /* Add some messages to the window */
    for (i = 0; i < 10; i++) {
        gchar *msg = g_strdup_printf ("Room %d", i);
        gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
        gtk_list_store_set (GTK_LIST_STORE (list_rooms), 
	                    &iter,
                            0, msg,
	                    -1);
	g_free (msg);
    }
}

void update_list_users() {
    GtkTreeIter iter;
    int i;

    /* Add some messages to the window */
    for (i = 0; i < 10; i++) {
        gchar *msg = g_strdup_printf ("User %d", i);
        gtk_list_store_append (GTK_LIST_STORE (list_users), &iter);
        gtk_list_store_set (GTK_LIST_STORE (list_users), 
                      &iter,
                            0, msg,
                      -1);
  g_free (msg);
    }
}

/* Create the list of "messages" */
static GtkWidget *create_list( const char * titleColumn, GtkListStore *model )
{
    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
    //GtkListStore *model;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    int i;
   
    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				    GTK_POLICY_AUTOMATIC, 
				    GTK_POLICY_AUTOMATIC);
   
    //model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
    gtk_widget_show (tree_view);
   
    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes (titleColumn,
                                                       cell,
                                                       "text", 0,
                                                       NULL);
  
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
	  		         GTK_TREE_VIEW_COLUMN (column));

    return scrolled_window;
}
   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static void insert_text()
{
   GtkTextIter iter;
 
   gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
   //gtk_text_buffer_insert (buffer, &iter, initialText,-1);
}
   
/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text( const char * initialText )
{
   GtkWidget *scrolled_window;
   GtkWidget *view;
   //GtkTextBuffer *buffer;

   view = gtk_text_view_new ();
   globalbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

   scrolled_window = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
		   	           GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);

   gtk_container_add (GTK_CONTAINER (scrolled_window), view);
   insert_text (globalbuffer, initialText);

   gtk_widget_show_all (scrolled_window);

   return scrolled_window;
}

static void enter_callbackSM( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("%s\n", entry_text);
  //char * host = strdup("localhost");
  //int port = 1234;

  //int sendCommand(char * host, int port, char * command, char * user,
   // char * password, char * args, char * response)
  // entry text -> insert command and assign
  //break up the entry text.
  // give it to send command.
  


}

static void enter_callbackER( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *entry_text;
  const gchar *entry_text2;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  entry_text2 = gtk_entry_get_text(GTK_ENTRY(entry));
  printf ("%s\n", entry_text);
  printf("%s\n",entry_text2);
  //char * host = strdup("localhost");
  //int port = 1234;

  //int sendCommand(char * host, int port, char * command, char * user,
   // char * password, char * args, char * response)
  // entry text -> insert command and assign
  //break up the entry text.
  // give it to send command.
  


}

static void enter_callbackCR( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  //printf ("%s\n", entry_text);
  //char * host = strdup("localhost");
  //int port = 1234;

  //int sendCommand(char * host, int port, char * command, char * user,
   // char * password, char * args, char * response)
  // entry text -> insert command and assign
  //break up the entry text.
  // give it to send command.
  


}

static void enter_callbackAU( GtkWidget *widget,
                            GtkWidget *entry)
{
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  char * tok = strdup(entry_text);
  char * user = strtok(tok," ");
  char * password = strtok(NULL," ");
  char response[100];
  char * host = strdup("localhost");
  int port = 1234;
  char * command = strdup("ADD-USER");
  char * args = strdup("EMPTY");
  printf("host = %s\n",host);
  printf("port = %d\n",port);
  printf("command = %s\n",command);
  printf("user = %s\n",user);
  printf("password = %s\n",password);
  sendCommand(host,port,command,user,password,args,response);
  printf("response = %s",response);
}

static void entry_toggle_editable( GtkWidget *checkbutton,
                                   GtkWidget *entry )
{
  gtk_editable_set_editable (GTK_EDITABLE (entry),
                             GTK_TOGGLE_BUTTON (checkbutton)->active);
}

static void entry_toggle_visibility( GtkWidget *checkbutton,
                                     GtkWidget *entry )
{
  gtk_entry_set_visibility (GTK_ENTRY (entry),
          GTK_TOGGLE_BUTTON (checkbutton)->active);
}

void send_button_clicked(GtkWidget *widget, gpointer data)
{
  //g_print("clicked\n");
  GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  GtkWidget *entry2;
  gint tmp_pos;
  gint tmp_pos2;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Send");
    //g_signal_connect (window2, "destroy",
     //                 G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect_swapped (window2, "delete-event",
      //                        G_CALLBACK (gtk_widget_destroy), 
       //                       window2);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackSM),
          entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,press enter");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    //g_signal_connect (vbox, "destroy",
    //                  G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect_swapped (vbox, "delete-event",
    //                          G_CALLBACK (gtk_widget_destroy), 
    //                          vbox);
    gtk_widget_show (entry);

    entry2 = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry2), 50);
    g_signal_connect (entry2, "activate",
          G_CALLBACK (enter_callbackSM),
          entry2);
    gtk_entry_set_text (GTK_ENTRY (entry2), "enter password,press enter");
    tmp_pos2= GTK_ENTRY (entry2)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry2),
              0, GTK_ENTRY (entry2)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry2, TRUE, TRUE, 0);
    gtk_widget_show (entry2);

    button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    g_signal_connect_swapped (button, "clicked",
            G_CALLBACK (gtk_widget_destroy),
            window2);
    gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);
    gtk_widget_set_can_default (button, TRUE);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);

    gtk_widget_show (window2);

}

void createuser_button_clicked(GtkWidget *widget, gpointer data) {
  GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  GtkWidget *entry2;
  gint tmp_pos;
  gint tmp_pos2;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Create User");
    //g_signal_connect (window2, "destroy",
     //                 G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect_swapped (window2, "delete-event",
      //                        G_CALLBACK (gtk_widget_destroy), 
       //                       window2);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackAU),
          entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,password,press enter");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    //g_signal_connect (vbox, "destroy",
    //                  G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect_swapped (vbox, "delete-event",
    //                          G_CALLBACK (gtk_widget_destroy), 
    //                          vbox);
    gtk_widget_show (entry);

    /*entry2 = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry2), 50);
    g_signal_connect (entry2, "activate",
          G_CALLBACK (enter_callbackAU),
          entry2);
    gtk_entry_set_text (GTK_ENTRY (entry2), "enter password,press enter");
    tmp_pos2= GTK_ENTRY (entry2)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry2),
              0, GTK_ENTRY (entry2)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry2, TRUE, TRUE, 0);
    gtk_widget_show (entry2); */

    button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    g_signal_connect_swapped (button, "clicked",
            G_CALLBACK (gtk_widget_destroy),
            window2);
    gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);
    gtk_widget_set_can_default (button, TRUE);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);

    gtk_widget_show (window2);
}

void enterroom_button_clicked(GtkWidget *widget, gpointer data) {
  GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  GtkWidget *entry2;
  GtkWidget * entry3;
  gint tmp_pos;
  gint tmp_pos2;
  gint tmp_pos3;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Enter Room");
    //g_signal_connect (window2, "destroy",
     //                 G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect_swapped (window2, "delete-event",
      //                        G_CALLBACK (gtk_widget_destroy), 
       //                       window2);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackER),
          entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,press enter");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    //g_signal_connect (vbox, "destroy",
    //                  G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect_swapped (vbox, "delete-event",
    //                          G_CALLBACK (gtk_widget_destroy), 
    //                          vbox);
    gtk_widget_show (entry);

    entry2 = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry2), 50);
    g_signal_connect (entry2, "activate",
          G_CALLBACK (enter_callbackER),
          entry2);
    gtk_entry_set_text (GTK_ENTRY (entry2), "enter password,press enter");
    tmp_pos2= GTK_ENTRY (entry2)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry2),
              0, GTK_ENTRY (entry2)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry2, TRUE, TRUE, 0);
    gtk_widget_show (entry2);

    entry3 = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry3), 50);
    g_signal_connect (entry3, "activate",
          G_CALLBACK (enter_callbackER),
          entry3);
    gtk_entry_set_text (GTK_ENTRY (entry3), "enter roomname,press enter");
    tmp_pos3= GTK_ENTRY (entry3)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry2),
              0, GTK_ENTRY (entry3)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry3, TRUE, TRUE, 0);
    gtk_widget_show (entry3);

    button = gtk_button_new_with_label ("DONE");
    g_signal_connect_swapped (button, "clicked",
            G_CALLBACK (gtk_widget_destroy),
            window2);
    gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);
    gtk_widget_set_can_default (button, TRUE);
    gtk_widget_grab_default (button);
    gtk_widget_show (button); 
    /*button = gtk_button_new ();
    g_signal_connect_swapped (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (enter_callbackER),window2);
    gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);
    gtk_widget_set_can_default (button, TRUE);
    gtk_widget_grab_default (button);
    gtk_widget_show (button); */

    gtk_widget_show (window2);
}

void createroom_button_clicked(GtkWidget * widget,gpointer data) {
  GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  GtkWidget *entry2;
  GtkWidget * entry3;
  gint tmp_pos;
  gint tmp_pos2;
  gint tmp_pos3;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Create Room");
    //g_signal_connect (window2, "destroy",
     //                 G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect_swapped (window2, "delete-event",
      //                        G_CALLBACK (gtk_widget_destroy), 
       //                       window2);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackCR),
          entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,password,room");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    //gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);

    gtk_widget_show (entry);
    gtk_widget_show (window2);
}

int main( int   argc,
          char *argv[] )
{
    GtkWidget *window;
    GtkWidget *list;
    GtkWidget *list2;
    GtkWidget *messages;
    GtkWidget *myMessage;


    gtk_init (&argc, &argv);

   
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Paned Windows");
    g_signal_connect (window, "destroy",
	              G_CALLBACK (gtk_main_quit), NULL);  
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (GTK_WIDGET (window), 450, 400);

    // Create a table to place the widgets. Use a 7x4 Grid (7 rows x 4 columns)
    GtkWidget *table = gtk_table_new (7, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    // Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
    list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    update_list_rooms();
    list = create_list ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), list, 2, 4, 0, 2);
    gtk_widget_show (list);

    list_users = gtk_list_store_new(1,G_TYPE_STRING);
    update_list_users();
    list2 = create_list("Users",list_users);
    gtk_table_attach_defaults(GTK_TABLE (table), list2, 0,2,0,2);
    gtk_widget_show(list2);

   
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    messages = create_text ("Peter: Hi how are you\nMary: I am fine, thanks and you?\nPeter: Fine thanks.\n");
    gtk_table_attach_defaults (GTK_TABLE (table), messages, 0, 4, 2, 5);
    gtk_widget_show (messages);
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 

    myMessage = create_text();
    gtk_widget_show (myMessage);

    // Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
    GtkWidget *send_button = gtk_button_new_with_label ("Send");
    gtk_table_attach_defaults(GTK_TABLE (table), send_button, 0, 1, 7,8);
    g_signal_connect(G_OBJECT(send_button), "clicked", G_CALLBACK(send_button_clicked), NULL);
    gtk_widget_show (send_button);

    GtkWidget *createuser_button = gtk_button_new_with_label("Create Account");
    gtk_table_attach_defaults(GTK_TABLE(table),createuser_button, 3, 4, 7, 8);
    g_signal_connect(G_OBJECT(createuser_button), "clicked", G_CALLBACK(createuser_button_clicked), NULL);
    gtk_widget_show(createuser_button);

    GtkWidget * enterroom_button = gtk_button_new_with_label("Enter Room");
    gtk_table_attach_defaults(GTK_TABLE(table),enterroom_button,1,2,7,8);
    g_signal_connect(G_OBJECT(enterroom_button), "clicked", G_CALLBACK(enterroom_button_clicked), NULL);
    gtk_widget_show(enterroom_button);

    GtkWidget * createroom_button = gtk_button_new_with_label("Create Room");
    gtk_table_attach_defaults(GTK_TABLE(table),createroom_button,2,3,7,8);
    g_signal_connect(G_OBJECT(createroom_button), "clicked", G_CALLBACK(createroom_button_clicked), NULL);
    gtk_widget_show(createroom_button);
    
    gtk_widget_show (table);
    gtk_widget_show (window);

    gtk_main ();

    return 0;
}

