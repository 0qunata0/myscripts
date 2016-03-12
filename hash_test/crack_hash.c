#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define HASHLEN 3

const EVP_MD *md;

void get_digest(unsigned char* md_value, char* str)
{
	EVP_MD_CTX *mdctx;
	int md_len, i;

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, str, strlen(str));
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
}

void get_random_string(char* str, size_t size)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyz";
	int i;

	for (i = 0; i < size; i++){
		int index = rand() % (sizeof charset - 1);
		str[i] = charset[index];
	}
	str[size] = '\0';
}

void print_summary(char* input_string, char* rand_string, unsigned char* ori_hash, unsigned char* found_hash, int count)
{
	int i;

	fprintf(stdout, "The %d attempts.\n", count);
	fprintf(stdout, "The input string: %s\n", input_string);
	fprintf(stdout, "The random string: %s\n", rand_string);
	fprintf(stdout, "The hash of input string: ");
	for (i = 0; i < HASHLEN; i++)
		fprintf(stdout, "%02x", ori_hash[i]);
	fprintf(stdout, "\n");
	fprintf(stdout, "The hash of random string: ");
	for (i = 0; i < HASHLEN; i++)
                fprintf(stdout, "%02x", found_hash[i]);
        fprintf(stdout, "\n");
}

int main(int argc, char** argv)
{
	if (argc != 3){
		fprintf(stderr, "Usage: %s input[5 values] digestname", argv[0]);
		fprintf(stderr, "This program will first hash the input string.\nAfter the retrieval of the hash, the program will start brute-force cracking the hash by randomly generating strings.\nFor the time sake, please only input a string that is shorter than 5 values.\n");
		exit(1);
	}

	OpenSSL_add_all_digests();

	char* input_string = argv[2];
	md = EVP_get_digestbyname(argv[1]);

	if (strlen(input_string) > 5){
		fprintf(stderr, "Error: Please enter a string that is shorter than 5 values. Otherwises, this program will run forever.\n");
		exit(1);
	}

	if (!md){
		fprintf(stderr, "Unknown message digest %s\n", argv[1]);
		exit(1);
	}
	
	unsigned char temp_val[EVP_MAX_MD_SIZE] = "";
	unsigned char hash_val[HASHLEN] = "";

	get_digest(temp_val, input_string);
	strncpy(hash_val, temp_val, HASHLEN);
	hash_val[HASHLEN] = '\0';
	EVP_cleanup();

	int len = strlen(input_string);
	char ran_string[len];
	int count = 1;

	for (;;){
		get_random_string(ran_string, len);

		unsigned char temp_arr[EVP_MAX_MD_SIZE] = "";
		unsigned char hash_temp[HASHLEN] = "";

		get_digest(temp_arr, ran_string);
		strncpy(hash_temp, temp_arr, HASHLEN);
		hash_temp[HASHLEN] = '\0';

		if (strncmp(hash_val, hash_temp, HASHLEN) == 0){
			print_summary(input_string, ran_string, hash_val, hash_temp, count);
			break;
		}

		count++;
		EVP_cleanup();
	}
	return 0;
}
