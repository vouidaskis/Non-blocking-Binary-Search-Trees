#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include<unistd.h>
#include "tree.c"

pthread_barrier_t mybarrier;


struct thread_data{
	long key_sum_counter; 
	int false_delete,false_insert,false_find;
	int true_delete,true_insert,true_find;
	int thread_id;
	struct stop_flag_and_operation_percentages *stop_operation;
	struct node *root;
	
};
struct stop_flag_and_operation_percentages{ 

	int max_key;
	int stop_experiment;
	int insert_percentage,delete_percentage,find_percentage; 
	

};

void *threadFunc(void *arg){

	struct thread_data *thread_data = (struct thread_data*)arg;
	int i, op, key;
	long int random_val, seed;
	struct drand48_data rand_buffer;
	long thread_id = (long) arg;
	seed = time(NULL) + thread_id;
	srand48_r(seed, &rand_buffer);
	// wait all thread
	pthread_barrier_wait(&mybarrier);
	
	//stop while when a certain of time passes
	while( thread_data->stop_operation->stop_experiment !=1 ){		
		lrand48_r(&rand_buffer, &random_val);
		int key = random_val %thread_data->stop_operation->max_key;
		lrand48_r(&rand_buffer, &random_val);
		int select_operation_probability = random_val % 100;
		
		if(select_operation_probability < thread_data->stop_operation->insert_percentage){
			if( insert(thread_data->root, key)){
				thread_data->true_insert = thread_data->true_insert + 1;
				thread_data->key_sum_counter = thread_data->key_sum_counter + key;
			}else thread_data->false_insert = thread_data->false_insert + 1;
			
		}else if(select_operation_probability < thread_data->stop_operation->insert_percentage + thread_data->stop_operation->delete_percentage){
		       	if( delete(thread_data->root, key)){

				thread_data->true_delete = thread_data->true_delete + 1;
				thread_data->key_sum_counter = thread_data->key_sum_counter - key;
			}else thread_data->false_delete = thread_data->false_delete + 1;
			
		}else{
			if(find(thread_data->root, key)){
				thread_data->true_find = thread_data->true_find + 1;

			}else thread_data->false_find = thread_data->false_find + 1;
		}
		
	}
	
	pthread_exit(NULL);
}

void main(int argc, char const* argsv[]){
char fname[256]; strcpy(fname, argsv[8]);

FILE *fp;
fp = fopen(fname, "w");
fprintf(fp,"trial,n_thread,time,threadops,max_key,first_key_sum_leaf,key_sum_leaf,key_sum_counter,find_probability,delete_probability,insert_probability,sum_insert_true,sum_insert_false,sum_find_true,sum_find_false,sum_delete_true, sum_delete_false, throughput\n");

	struct info *rinfo = (struct info *)malloc(sizeof(struct info));
	struct info *linfo = (struct info *)malloc(sizeof(struct info));
	struct info *info = (struct info *)malloc(sizeof(struct info));
	struct node *root = (struct node *)malloc(sizeof(struct node));
	struct node *r1 = (struct node *)malloc(sizeof(struct node));
	struct node *l1 = (struct node *)malloc(sizeof(struct node));
	info->info_node[0] = NULL;
	info->info_node[1] = NULL;
	info->info_node[2] = NULL;
	info->pinfo = NULL;
	linfo->info_node[0] = NULL;
	linfo->info_node[1] = NULL;
	linfo->info_node[2] = NULL;	
	rinfo->info_node[0] = NULL;
	rinfo->info_node[1] = NULL;
	rinfo->info_node[2] = NULL;		
	rinfo->pinfo = NULL;
	linfo->pinfo = NULL;
	l1->left = NULL;
	l1->right = NULL;
	r1->left = NULL;
	r1->right = NULL;
	l1->info = linfo;

	r1->info = rinfo;
	root->key=INT_MAX;
	r1->key = INT_MAX;
        l1->key = INT_MAX-1;
	root->right = r1;
	root->left = l1;
	root->info = info;
	int size_tree = 0;
	int i;
	while(size_tree < (atoi(argsv[1]) *atoi(argsv[6])/ (atoi(argsv[1]) + atoi(argsv[1])))){

		for(i=0; i<atoi(argsv[6])/10; i++){
			int key = rand()%atoi(argsv[6]);
			if(insert(root, key))size_tree = size_tree +1; 
		}

	}
	
	int before_key_sum_leaf = inorder_return_sum(root->left->left);
	int number_of_threads;
	int experiment_time = atoi(argsv[7]);

	number_of_threads = atoi(argsv[5]);
	pthread_t id[number_of_threads];

	struct stop_flag_and_operation_percentages *stopw = (struct stop_flag_and_operation_percentages *)malloc(sizeof(struct 				stop_flag_and_operation_percentages));
	pthread_barrier_init(&mybarrier, NULL, number_of_threads + 1);
	stopw->max_key = atoi(argsv[6]);
	stopw->stop_experiment = 0;
	stopw->find_percentage = atoi(argsv[3]);
	stopw->delete_percentage = atoi(argsv[2]);
	stopw->insert_percentage = atoi(argsv[1]);
	struct thread_data *temp1 = calloc(number_of_threads ,sizeof(struct thread_data));

	
	for (i=0; i<number_of_threads ;i++){ // put thread with information
		
		temp1[i].root = root;
		temp1[i].key_sum_counter = 0;
		temp1[i].false_delete = 0;
		temp1[i].false_insert = 0;
		temp1[i].false_find = 0;
		temp1[i].true_delete = 0;
		temp1[i].true_insert = 0;
		temp1[i].true_find = 0;
		temp1[i].thread_id = i;
		temp1[i].stop_operation = stopw;

		if(pthread_create(&id[i],NULL, threadFunc, (void *)&temp1[i])!=0){//create the thread.
			printf("Problem creating thread.\n");
			exit(1);
		}

	}
	//it shows how creat all thread
	pthread_barrier_wait(&mybarrier);
	struct timespec start,end ;
	
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	sleep(experiment_time);
	stopw->stop_experiment=1;	
	

	//wait all thread
	for(i=0; i<number_of_threads; i++){//
		
		if(pthread_join(id[i],NULL)!=0){
			printf("Error when joining thead\n");
			exit(1);
		}
		
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	
	long key_sum_cunter_leaf = 0;
	int total_leaf_key = 0;
	int sum_insert_true = 0;
	int sum_find_true = 0;
	int sum_delete_true = 0;
	int sum_insert_false = 0;
	int sum_delete_false = 0;
	int sum_find_false = 0;
	long sum_time = 0;
	long threadops = 0;
	long throughput = 0;
	long sum_key = 0;




	for(i=0; i<number_of_threads; i++){
		
		sum_insert_true = sum_insert_true + temp1[i].true_insert;
		sum_delete_true = sum_delete_true + temp1[i].true_delete;
		sum_find_true = sum_find_true + temp1[i].true_find;
		sum_insert_false = sum_insert_false + temp1[i].false_insert;
		sum_delete_false = sum_delete_false + temp1[i].false_delete;
		sum_find_false = sum_find_false + temp1[i].false_find;
		printf("%d\n", temp1[i].key_sum_counter);
		sum_key = sum_key + temp1[i].key_sum_counter;
	}
	
total();
	threadops = sum_insert_true +sum_delete_true +  sum_find_true + sum_insert_false + sum_delete_false + sum_find_false;
	double time = (end.tv_sec-start.tv_sec) +0.000000001*(end.tv_nsec - start.tv_nsec);
	key_sum_cunter_leaf = inorder_return_sum(root->left->left);
	total_leaf_key = inorder_return_total(root->left->left);
	throughput = threadops/time;

	printf("total key sum (tree):\t  %ld\n",key_sum_cunter_leaf - before_key_sum_leaf); 
	printf("total key sum (counters): %ld\n",sum_key);

	fprintf(fp,"%d,%d,%f,%ld,%d,%d,%d,%ld,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d,%d,%f\n",atoi(argsv[7]),number_of_threads,time,threadops,atoi(argsv[6]),before_key_sum_leaf,total_leaf_key,key_sum_cunter_leaf,0.01*atoi(argsv[3]),0.01*atoi(argsv[2]),0.01*atoi(argsv[1]),sum_insert_true,sum_insert_false,sum_find_true,sum_find_false,sum_delete_true, sum_delete_false,throughput/1000000);

}

