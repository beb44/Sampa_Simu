extern "C"{
 int create_dualsampa(int ad1,int ad2);
 int delete_dualsampa(int id);
 int set_user_handler(int id,void (*user_handler)(int));
 int reset_frames(int id);
 int add_data(int id,int sampaid,int len,unsigned short *buffer);
 int send_frames(int id);
 }
