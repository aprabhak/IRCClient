
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
#include <stdarg.h>

GtkListStore * list_rooms;
GtkListStore * list_users;
GtkListStore * list_messages;
GtkWidget *tree_view;
char * host = strdup("localhost");
int port = 1234;
//char * host;
char * sport;
//int port;
char * globaluser;
char * globalpassword;
char * globalroom;
char * globalroom2;
GtkTextBuffer *globalbuffer;
//GtkTextBuffer *globalbuffer;

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

  printf("response:%s\n", response);

  close(sock);

  return 1;
}

int sendCommand2(char * host, int port, char * command, char * user,
    char * password, char * args,char * message, char * response) {
  int sock = open_client_socket( host, port);

  // Send command
  write(sock, command, strlen(command));
  write(sock, " ", 1);
  write(sock, user, strlen(user));
  write(sock, " ", 1);
  write(sock, password, strlen(password));
  write(sock, " ", 1);
  write(sock, args, strlen(args));
  write(sock, " ", 1);
  write(sock, message, strlen(message));
  write(sock, "\r\n",2);

  // Keep reading until connection is closed or MAX_REPONSE
  int n = 0;
  int len = 0;
  while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
    len += n;
  }

  printf("response:%s\n", response);

  close(sock);

  return 1;
}

void update_list_rooms() {
  GtkTreeIter iter;
  int i;
  char response[MAX_RESPONSE];
  char * command = strdup("LIST-ROOMS");
  char * args = strdup("");
  memset(response,0,sizeof(response));
  if (globaluser == NULL) {
    return;
  }
  sendCommand(host,port,command,globaluser,globalpassword,args,response);
  gtk_list_store_clear(list_rooms);
  char * str;
  str = strtok(response, "\r\n");
  while (str != NULL) {
    gchar * msg = g_strdup_printf("%s",str);
    str = strtok(NULL,"\r\n");
    gtk_list_store_append(GTK_LIST_STORE(list_rooms),&iter);
    gtk_list_store_set(GTK_LIST_STORE(list_rooms),&iter,0,msg,-1);
    g_free(msg);
  }
}

void update_list_messages() {
  GtkTreeIter iter;
  int i;
  char response[MAX_RESPONSE];
  char * command = strdup("GET-MESSAGES");
  memset(response,0,sizeof(response));
  if (globaluser == NULL) {
    return;
  }

  if (globalroom2 == NULL) {
    return;
  }
  char * num = strdup("0");
  sendCommand2(host,port,command,globaluser,globalpassword,num,globalroom2,response);
  gtk_list_store_clear(list_messages);
  char * str;
  str = strtok(response, "\r\n");
  while (str != NULL) {
    gchar * msg = g_strdup_printf("%s",str);
    str = strtok(NULL,"\r\n");
    gtk_list_store_append(GTK_LIST_STORE(list_messages),&iter);
    gtk_list_store_set(GTK_LIST_STORE(list_messages),&iter,0,msg,-1);
    g_free(msg);
  }
}

void update_list_users() {
    /*GtkTreeIter iter;
    int i;

    // Add some messages to the window
    for (i = 0; i < 10; i++) {
        gchar *msg = g_strdup_printf ("User %d", i);
        gtk_list_store_append (GTK_LIST_STORE (list_users), &iter);
        gtk_list_store_set (GTK_LIST_STORE (list_users), 
                      &iter,
                            0, msg,
                      -1);
  g_free (msg);
    }  */
  GtkTreeIter iter;
  int i;
  char response[MAX_RESPONSE];
  char * command = strdup("GET-USERS-IN-ROOM");
  memset(response,0,sizeof(response));
  //char * args = strdup("room1");
  if (globaluser == NULL) {
    return;
  }
  if (globalroom == NULL) {
    return;
  }
  sendCommand(host,port,command,globaluser,globalpassword,globalroom,response);
  gtk_list_store_clear(list_users);
  char * str;
  str = strtok(response, "\r\n");
  while (str != NULL) {
    gchar * msg = g_strdup_printf("%s",str);
    str = strtok(NULL,"\r\n");
    gtk_list_store_append(GTK_LIST_STORE(list_users),&iter);
    gtk_list_store_set(GTK_LIST_STORE(list_users),&iter,0,msg,-1);
    g_free(msg);
  }
}

/* Create the list of "messages" */
static GtkWidget *create_list( const char * titleColumn, GtkListStore *model )
{
    GtkWidget *scrolled_window;
    //GtkWidget *tree_view;
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
    g_signal_connect(tree_view, "row-activated", G_CALLBACK(update_list_users), NULL);
    return scrolled_window;
}
   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static void insert_text( GtkTextBuffer *buffer, const char * initialText )
{
   GtkTextIter iter;
 
   gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
   gtk_text_buffer_insert (buffer, &iter, initialText,-1);
}

static void insert_text2()
{
   GtkTextIter iter;
 
    gtk_text_buffer_create_tag(globalbuffer, "blue_fg", "foreground", "blue", NULL);
   gtk_text_buffer_get_iter_at_offset (globalbuffer, &iter, 0);
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

static GtkWidget *create_text2(void)
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
   insert_text2 ();

   gtk_widget_show_all (scrolled_window);

   return scrolled_window;
}

static void enter_callbackAU( GtkWidget *widget,
                            GtkWidget *entry)
{
  GtkWidget *window2;
  GtkWidget *label;
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  char * tok = strdup(entry_text);
  char * user = strtok(tok," ");
  char * password = strtok(NULL," ");
  char response[100];
  char * command = strdup("ADD-USER");
  char * args = strdup("EMPTY");

  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 200, 100);
    gtk_window_set_title (GTK_WINDOW (window2), "response");
    //gtk_widget_show (vbox);

  printf("host = %s\n",host);
  printf("port = %d\n",port);
  printf("command = %s\n",command);
  printf("user = %s\n",user);
  printf("password = %s\n",password);
  globaluser = strdup(user);
  globalpassword = strdup(password);
  memset(response,0,sizeof(response));
  sendCommand(host,port,command,user,password,args,response);
  printf("response = %s",response);
  GtkWidget *dialog;

  dialog = gtk_message_dialog_new(GTK_WINDOW(window2),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",response);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
} 

static void adduser_button_clicked( GtkWidget *widget,
                            gpointer data)
{
  GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  gint tmp_pos;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Create User");
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
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);
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

static void enter_callbackSM(GtkWidget *widget,GtkWidget * entry) {
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  char * tok = strdup(entry_text);
  char * user = strtok(tok," ");
  char * password = strtok(NULL," ");
  char * room = strtok(NULL," ");
  char response[100];
  char * host = strdup("localhost");
  int port = 1234;
  char * command = strdup("SEND-MESSAGE");
  //char * args = strdup("EMPTY");
  printf("host = %s\n",host);
  printf("port = %d\n",port);
  printf("command = %s\n",command);
  printf("user = %s\n",user);
  printf("password = %s\n",password);
  printf("room = %s\n",room);
  sendCommand(host,port,command,user,password,room,response);
  printf("response = %s\n",response);
}

static void send_button_clicked(GtkWidget *widget,gpointer data) {
  GtkTextIter start;
  GtkTextIter end;
  gchar * text;
  char * message;

  gtk_text_buffer_get_start_iter(globalbuffer,&start);
  gtk_text_buffer_get_end_iter(globalbuffer,&end);

  text = gtk_text_buffer_get_text(globalbuffer,&start,&end,FALSE);
  g_print("%s\n",text);
  message = strdup(text);
  //const gchar * entry_text;
  //char *mezzage;
  //entry_text = gtk_entry_get_text(GTK_ENTRY(myMessage));
  //memset(mezzage,0,sizeof(mezzage));
  //mezzage = strdup(entry_text);
  char * command = strdup("SEND-MESSAGE");
  char response[100];
  //memset(response,0,sizeof(response));
  sendCommand2(host,port,command,globaluser,globalpassword,globalroom2,message,response);
  printf("response = %s\n",response);
  /*GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  gint tmp_pos;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Send message");
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    cutg_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackSM),
          entry);cut
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,password,room,press enter");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);
    //button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    button = gtk_button_new_with_label("send");
    cutg_signal_connect_swapped (button, "clicked",
            G_CALLBACK (gtk_widget_destroy),
            window2); cut
      //g_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(enter_callbackSM),(gpointer)entry);
    gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);
    gtk_widget_set_can_default (button, TRUE);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);

    gtk_widget_show (window2);*/
}

static void enter_callbackCR(GtkWidget *widget,GtkWidget * entry) {
  GtkWidget *window2;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 200, 100);
    gtk_window_set_title (GTK_WINDOW (window2), "response");
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  char * tok = strdup(entry_text);
  char * user = strtok(tok," ");
  char * password = strtok(NULL," ");
  char * room = strtok(NULL," ");
  char response[100];
  globaluser = strdup(user);
  globalpassword = strdup(password);
  char * host = strdup("localhost");
  int port = 1234;
  char * command = strdup("CREATE-ROOM");
  //char * args = strdup("EMPTY");
  printf("host = %s\n",host);
  printf("port = %d\n",port);
  printf("command = %s\n",command);
  printf("user = %s\n",user);
  printf("password = %s\n",password);
  printf("room = %s\n",room);
  memset(response,0,sizeof(response));
  sendCommand(host,port,command,user,password,room,response);
  /*if (strcmp(response,"OK\r\n") == 0) {
    update_list_rooms();
  } */
  //printf("response = %s\n",response);
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",response);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void createroom_button_clicked(GtkWidget *widget,gpointer data) {
  GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  gint tmp_pos;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Create Room");
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackCR),
          entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,password,room,press enter");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);
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

static void enter_callbackER(GtkWidget *widget,GtkWidget * entry) {
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  char * tok = strdup(entry_text);
  char * user = strtok(tok," ");
  char * password = strtok(NULL," ");
  char * args = strtok(NULL," ");
  char response[100];
  char * host = strdup("localhost");
  int port = 1234;
  char * command = strdup("ENTER-ROOM");
  globaluser = strdup(user);
  globalpassword = strdup(password);
  globalroom2 = strdup(args);
  //globalroom = strdup(args);
  printf("host = %s\n",host);
  printf("port = %d\n",port);
  printf("command = %s\n",command);
  printf("user = %s\n",user);
  printf("password = %s\n",password);
  printf("room = %s\n",args);
  sendCommand(host,port,command,user,password,args,response);
  char * command2 = strdup("SEND-MESSAGE");
  char * sub1 = strdup(user);
  char *sub2 = strdup(" entered room");
  char * message = strcat(sub1,sub2);
  memset(response,0,sizeof(response));
  sendCommand2(host,port,command2,user,password,args,message,response);
  printf("response = %s\n",response);
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",response);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void enterroom_button_clicked(GtkWidget *widget,gpointer data) {
  GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  gint tmp_pos;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Enter Room");
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackER),
          entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,password,room,press enter");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);
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

static void enter_callbackLR(GtkWidget *widget,GtkWidget * entry) {
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  char * tok = strdup(entry_text);
  char * user = strtok(tok," ");
  char * password = strtok(NULL," ");
  char * room = strtok(NULL," ");
  char response[100];
  char * host = strdup("localhost");
  int port = 1234;
  char * command = strdup("LEAVE-ROOM");
  //char * args = strdup("EMPTY");
  printf("host = %s\n",host);
  printf("port = %d\n",port);
  printf("command = %s\n",command);
  printf("user = %s\n",user);
  printf("password = %s\n",password);
  printf("room = %s\n",room);
    char * command2 = strdup("SEND-MESSAGE");
  char * sub1 = strdup(user);
  char *sub2 = strdup(" left room");
  char * message = strcat(sub1,sub2);
  sendCommand2(host,port,command2,user,password,room,message,response);
  printf("response = %s\n",response);
  printf("response = %s\n",response);
  memset(response,0,sizeof(response));
  sendCommand(host,port,command,user,password,room,response);
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",response);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

}

static void leaveroom_button_clicked(GtkWidget *widget,gpointer data) {
   GtkWidget *window2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *entry;
  gint tmp_pos;
  window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window2), 300, 200);
    gtk_window_set_title (GTK_WINDOW (window2), "Leave Room");
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window2), vbox);
    gtk_widget_show (vbox);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 50);
    g_signal_connect (entry, "activate",
          G_CALLBACK (enter_callbackLR),
          entry);
    gtk_entry_set_text (GTK_ENTRY (entry), "enter username,password,room,press enter");
    tmp_pos = GTK_ENTRY (entry)->text_length;
    gtk_editable_select_region (GTK_EDITABLE (entry),
              0, GTK_ENTRY (entry)->text_length);
    gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
    gtk_widget_show (entry);
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

gboolean room_timeout(gpointer data) {
  update_list_rooms();
  return 1;
}

gboolean user_timeout(gpointer data) {
  update_list_users();
  return 1;
}

gboolean message_timeout(gpointer data) {
  update_list_messages();
  return 1;
}

void  on_changed(GtkWidget *widget, gpointer label) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;


  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(widget), &model, &iter)) {

    gtk_tree_model_get(model, &iter, 0, &value,  -1);
    globalroom = strdup(value);
    printf("value =%s\n",value);
    g_free(value);
  }

}

int main( int   argc,
          char *argv[] )
{
    GtkWidget *window;
    GtkWidget *list;
    GtkWidget *list2;
    GtkWidget *list3;
    GtkWidget *messages;
    GtkWidget *myMessage;
    GtkTreeSelection *selection; 

    gtk_init (&argc, &argv);
   
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Client");
    g_signal_connect (window, "destroy",
	              G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (GTK_WIDGET (window), 550, 450);

    // Create a table to place the widgets. Use a 7x4 Grid (7 rows x 4 columns)
    GtkWidget *table = gtk_table_new (7, 5, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    // Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
    list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    //update_list_rooms();
    list = create_list ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), list, 2, 4, 0, 2);
    gtk_widget_show (list);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    g_signal_connect(selection, "changed", 
      G_CALLBACK(on_changed), NULL);

    list_users = gtk_list_store_new(1, G_TYPE_STRING);
    //update_list_users();
    list2 = create_list("Users",list_users);
    gtk_table_attach_defaults (GTK_TABLE (table), list2, 0, 2, 0, 2);
    gtk_widget_show (list2);
   
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    //messages = create_text ("Peter: Hi how are you\nMary: I am fine, thanks and you?\nPeter: Fine thanks.\n");
    //gtk_table_attach_defaults (GTK_TABLE (table), messages, 0, 4, 2, 5);
    //gtk_widget_show (messages);
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive)
    list_messages = gtk_list_store_new(1, G_TYPE_STRING);
    list3 = create_list("Messages",list_messages);
    gtk_table_attach_defaults (GTK_TABLE (table), list3, 0, 4, 2, 5);
    gtk_widget_show (list3);


    //myMessage = create_text ("I am fine, thanks and you?\n");
    myMessage = create_text2();
    //gtk_entry_set_max_length (GTK_ENTRY (myMessage), 50);
    gtk_table_attach_defaults (GTK_TABLE (table), myMessage, 0, 4, 5, 7);
    gtk_widget_show (myMessage);

    // Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
    GtkWidget *send_button = gtk_button_new_with_label ("Send");
    gtk_table_attach_defaults(GTK_TABLE (table), send_button, 0, 1, 7, 8);
    g_signal_connect(send_button,"clicked",G_CALLBACK(send_button_clicked),NULL); 
    gtk_widget_show (send_button);

    GtkWidget *adduser_button = gtk_button_new_with_label("Create account");
    gtk_table_attach_defaults(GTK_TABLE (table), adduser_button,1,2,7,8);
    g_signal_connect (adduser_button, "clicked",G_CALLBACK (adduser_button_clicked), NULL);
    gtk_widget_show(adduser_button); 


    GtkWidget *createroom_button = gtk_button_new_with_label("Create room");
    gtk_table_attach_defaults(GTK_TABLE (table), createroom_button,2,3,7,8);
    g_signal_connect (createroom_button, "clicked",G_CALLBACK (createroom_button_clicked), NULL);
    gtk_widget_show(createroom_button);


    GtkWidget *enterroom_button = gtk_button_new_with_label("Enter room");
    gtk_table_attach_defaults(GTK_TABLE (table), enterroom_button,3,4,7,8);
    g_signal_connect(enterroom_button,"clicked",G_CALLBACK(enterroom_button_clicked),NULL);
    gtk_widget_show(enterroom_button);


    GtkWidget *leaveroom_button = gtk_button_new_with_label("Leave room");
    gtk_table_attach_defaults(GTK_TABLE (table), leaveroom_button,4,5,7,8);
    g_signal_connect(leaveroom_button,"clicked",G_CALLBACK(leaveroom_button_clicked),NULL);
    gtk_widget_show(leaveroom_button);

    
    gtk_widget_show (table);
    gtk_widget_show (window);
    g_timeout_add_seconds(5,room_timeout,window);
    g_timeout_add_seconds(5,message_timeout,window);
    g_timeout_add_seconds(5,user_timeout,window);
    //g_timeout_add_seconds(5,room_timeout,window);
    //g_timeout_add_seconds(7,user_timeout,window);
    //g_timeout_add_seconds(9,message_timeout,window);

    gtk_main ();

    return 0;
}