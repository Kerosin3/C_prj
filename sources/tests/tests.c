#include "tests.h"
#include <assert.h>
#include <sodium/crypto_generichash.h>
#include <sodium/crypto_sign.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>

int test_hpoint_from_2msg_creation();
int test_hashG_node_creation();
int test_process_messages();

int test_create_tree();
void tests()
{
    int result = 0;
    /*result+=test_valid_messages();
    result+=test_INvalid_messages();
    result+=test_hash();
    test_hash_merging();*/
    // result+=test_process_messages();
    //	test_create_tree();
    // solve_puzlev2(2);
    //	setup_client();

    //      start_server(12345);
    //
    //
    // test_cleanup_message();
    // test_message_creation_and_validation();
    //	test_create_and_destroy_hpoint();
    // test_process_messages_L1_v2();
    //       create_test_messages(9);
    test_mekrle_proof();
    (!result) ? printf("ALL TESTS PASSED OK\n") : printf("SOME ERRORS WHILE TESTING OCCURRED!\n");
}

int proof_message(unsigned long long EXPONENT, size_t msg_num, layer_hp *Layers_pointer)
{
    size_t req_msg_first_nodeN = (msg_num >> 1LU); //assign node number
    int flip = (req_msg_first_nodeN % 2LU) ? 0 : 1; // flip hpoint if % is true num is level 1 num
    //flip = 1;
    printf("TESTING N MSG:%lu flip = %d \n",req_msg_first_nodeN,flip);
    size_t UPLOAD_req_msg_first_nodeN = req_msg_first_nodeN + 1LU;
    printf("level:%lu\n", (Layers_pointer[EXPONENT - 1]).level);
    //     ((*(Layers_pointer[EXPONENT-1].main_pointer))->messages.smsg_p1-> )
    // check ground level, TAKE BOTH MESSAGES
    unsigned char *hash_msg1_f =
        calc_hash(*(*(Layers_pointer[EXPONENT - 1].main_pointer[req_msg_first_nodeN])).messages.smsg_p1);
    //user_keys uk = create_key_pair();
    //unsigned char *hash_msg1_f =
    //    calc_hash(*ls_get_a_signed_msg(uk));

    unsigned char* Shash2 = calloc(crypto_generichash_BYTES,sizeof(unsigned char));
    unsigned char *hash_msg2_f =
        calc_hash(*(*(Layers_pointer[EXPONENT - 1].main_pointer[req_msg_first_nodeN])).messages.smsg_p2);
    unsigned char *Shash = merge_2hashses(hash_msg1_f, hash_msg2_f); // CALC SHASH
  //   printf("claced hash S ground level:\n");
//     DumpHex(Shash, crypto_generichash_BYTES);
    // printf("written hash:\n");
    // DumpHex(((*(Layers_pointer[EXPONENT-1].main_pointer[req_msg_first_nodeN])).hash),crypto_generichash_BYTES);
    int ret_gr_lev = 0;
    //ret_gr_lev = memcmp(Shash, ((*(Layers_pointer[EXPONENT - 1].main_pointer[req_msg_first_nodeN])).hash),
    //                    crypto_generichash_BYTES); // compare message itself
/*    printf("msg 1 hash:\n");
    DumpHex(hash_msg1_f, crypto_generichash_BYTES);
    printf("h\n");
    DumpHex(((hash_point_p) ((*(Layers_pointer[EXPONENT - 2].main_pointer[req_msg_first_nodeN])).hpoint1 ) )->hash,crypto_generichash_BYTES);
    printf("h\n");
    DumpHex(((hash_point_p) ((*(Layers_pointer[EXPONENT - 2].main_pointer[req_msg_first_nodeN])).hpoint2 ) )->hash,crypto_generichash_BYTES);
    */
/*
    for (size_t i = 0; i<(Layers_pointer[EXPONENT-1].size) ; i++) {
	    printf("%lu'th Shash\n",i);
	    DumpHex( ((*(Layers_pointer[EXPONENT - 1].main_pointer[i])).hash   )  ,crypto_generichash_BYTES);
    }

    for (size_t i = 0; i<(Layers_pointer[EXPONENT-2].size) ; i++) {
	printf("layer -2 s hash:\n");
	DumpHex(  ((hash_point_p)((*(Layers_pointer[EXPONENT - 2].main_pointer[i])).hpoint1))->hash ,crypto_generichash_BYTES  );

    }
    for (size_t i = 0; i<(Layers_pointer[EXPONENT-2].size) ; i++) {
	int z = memcmp( ((*(Layers_pointer[EXPONENT - 2].main_pointer[i]))).hash ,Shash, crypto_generichash_BYTES   );
	//int z = memcmp( ((hash_point_p) ((*(Layers_pointer[EXPONENT - 2].main_pointer[i])).hpoint1 ) )->hash,Shash, crypto_generichash_BYTES   );
	if (!z) {
		printf("----------------------------->found! %lu\n",i);
		break;
	}
    }
*/
    // CHECK SHASH 
    if (flip){
    ret_gr_lev = memcmp(Shash, ((hash_point_p) ((*(Layers_pointer[EXPONENT - 2].main_pointer[req_msg_first_nodeN>>1]))).hash)->hpoint1  ,
                        crypto_generichash_BYTES); // compare message itself
    } else {
    ret_gr_lev = memcmp(Shash, ((hash_point_p) ((*(Layers_pointer[EXPONENT - 2].main_pointer[req_msg_first_nodeN>>1]))).hash)->hpoint2  ,
                        crypto_generichash_BYTES); // compare message itself

    }
    if (ret_gr_lev)
    {
        printf("not matched hash!\n");
        return 0;
    }
    else
    {
        printf("ground level matched!\n");
	//return 1;
    }
    /*
    memset(
	    ((hash_point_p)((*(Layers_pointer[4].main_pointer[req_msg_first_nodeN])).hpoint2))->hash,
	    0,
	    crypto_generichash_BYTES);
	    */
    free(Shash);
    //-----------------------------------------//
    printf("\n");
    int Sret = 0;
    int rez = 0;
    for (int i = EXPONENT - 2; i >= 0; --i)
    {
        printf("I=%d\n", i);
        req_msg_first_nodeN >>= 1; // SHIFT
	printf("msg test N:%lu\n",req_msg_first_nodeN);
	if (i==0){
		printf("calc merkle root!\n");
		break;
	}
	Shash =
            merge_2hashses(
			    (*(Layers_pointer[i].main_pointer[req_msg_first_nodeN])).hash, // take cur node
			    (*(Layers_pointer[i].main_pointer[req_msg_first_nodeN+1])).hash // take neighbor node
			     );

       // rez = memcmp(Shash,( (((*(Layers_pointer[i - 1LU].main_pointer[req_msg_first_nodeN>>1]))).hash))    ,
        //                crypto_generichash_BYTES); // compare message itself
/*	
	if (flip){
        rez = memcmp(Shash,( (((*(Layers_pointer[i - 1LU].main_pointer[req_msg_first_nodeN>>1]))).hash))    ,
                        crypto_generichash_BYTES); // compare message itself
        } else {
        rez = memcmp(Shash,( (((*(Layers_pointer[i - 1LU].main_pointer[req_msg_first_nodeN>>1]))).hash))    ,
                        crypto_generichash_BYTES); // compare message itself

//        rez = memcmp(Shash,( ((hash_point_p)(((*(Layers_pointer[i - 1LU].main_pointer[req_msg_first_nodeN>>1]))).hpoint2))->hash)    ,
    //                    crypto_generichash_BYTES); // compare message itself
	}
//	rez = memcmp(Shash, ((hash_point_p) ((*(Layers_pointer[i - 1LU].main_pointer[req_msg_first_nodeN>>1]))).hash)->hpoint2  ,
  //                      crypto_generichash_BYTES); // compare message itself

	printf("hehe\n");
	//int rez = memcmp( 
	//		Shash,
	//		(*(Layers_pointer[i-1].main_pointer[req_msg_first_nodeN>>1])).hash, // take val from level upper 
	//		crypto_generichash_BYTES);
        
	 Shash =
            merge_2hashses(((hash_point_p)((*(Layers_pointer[i].main_pointer[req_msg_first_nodeN])).hpoint1))->hash,
                           ((hash_point_p)((*(Layers_pointer[i].main_pointer[req_msg_first_nodeN])).hpoint2))->hash);
        int rez =
            memcmp(Shash, (((*(Layers_pointer[i].main_pointer[req_msg_first_nodeN])).hash)), crypto_generichash_BYTES);
	*/
	if (rez) {
		printf("validation failed in %d layer, %lu message\n",i,req_msg_first_nodeN);
		return 1;
	}
        free(Shash);
        Sret += rez;
    }
    printf("EXponen here:%zu\n",req_msg_first_nodeN);
    DumpHex( ((hash_point_p) ((*(Layers_pointer[req_msg_first_nodeN].main_pointer))->hash )
          )->hash   ,crypto_generichash_BYTES);

    free(hash_msg2_f);
    free(hash_msg1_f);
    free(Shash2);
    Sret += ret_gr_lev;
    printf("RESULT %d\n", Sret);
    if (Sret)
        return 1;
    return 0;
}

int test_mekrle_proof()
{
    unsigned long long EXPONENT = 9;
    //-----create basic structures
    unsigned long long n_msg = (1LLU << EXPONENT); //  create 2^9 messages
    layer_hp *L_arrays[EXPONENT];
    layer_hp L_arrays_p[EXPONENT]; // for free
    printf("n msg :%llu\n", n_msg);

    user_keys uk = create_key_pair();
    signed_message_t **msg_arr = calloc(n_msg, sizeof(signed_message_t *));
    //----fill messages
    size_t i = 0;
    for (i = 0; i < n_msg; i++)
    {
        msg_arr[i] = ls_get_a_signed_msg(uk); // pointer to message
        validate_a_message(*msg_arr[i], uk.pk);
    }
    //-----------------------------------
    // CREATE BASE LAYER
    L_arrays[EXPONENT - 1] = process_s_messagesV2(n_msg, msg_arr);
    L_arrays_p[EXPONENT - 1] = *L_arrays[EXPONENT - 1]; // store pointer
    n_msg >>= 1;                                        // devide by 2
    printf("N OF LEVEL 0 HASH NODES %llu\n", n_msg);
    //--------------------------
    // create intermideate layers
    printf("filling intermideate layers\n");
    fill_intermediate_levels(EXPONENT, &n_msg, L_arrays, L_arrays_p); // done

    printf("merkle root calced:\n");
    DumpHex(((*(L_arrays_p[0].main_pointer))->hash), crypto_generichash_BYTES);
    //printf("level:%lu\n", (L_arrays_p[0]).level);
    //printf("firs msg:\n");
    //DumpHex(msg_arr[0]->message, msg_arr[0]->length);
    //printf("acces via structs\n");
    //printf("level:%lu\n", (L_arrays_p[EXPONENT - 1]).level);
    //DumpHex(((*(L_arrays_p[EXPONENT - 1].main_pointer))->messages.smsg_p1->message),
    //        ((*(L_arrays_p[EXPONENT - 1].main_pointer))->messages.smsg_p1->length));
    /*for (size_t i = 0; i<256; i++) {
    proof_message(EXPONENT, i, L_arrays_p);
    }*/
    proof_message(EXPONENT, 10, L_arrays_p);
    //
    // free rootlevel
    for (size_t i = 0; i < EXPONENT; i++)
    {
        destoroy_a_layer(L_arrays[i]);
    }
    //     destoroy_a_layer(&L_arrays_p[EXPONENT-1]); // destroy level
    for (i = 0; i < (1LLU << EXPONENT); i++)
    {
        destroy_signed_message(msg_arr[i]);
    }
    free(msg_arr); // free conrainer for messages
}

// ok!
int test_message_creation_and_validation()
{

    user_keys uk = create_key_pair();
    signed_message_t **msg_arr = calloc(512, sizeof(signed_message_t *));
    size_t i = 0;
    int rez = 0;
    for (i = 0; i < 512; i++)
    {
        msg_arr[i] = ls_get_a_signed_msg(uk);
        rez += validate_a_message(*msg_arr[i], uk.pk);
    }
    printf("rez is %d\n", rez);

    for (i = 0; i < 512; i++)
    {
        destroy_signed_message(msg_arr[i]);
    }
    free(msg_arr);
    return (rez == 512) ? 0 : 1;
}

// ok!
int test_cleanup_message()
{

    user_keys uk = create_key_pair();
    signed_message_t *msg;
    msg = ls_get_a_signed_msg(uk); // generate random
    DumpHex(msg->message, msg->length);
    validate_a_message(*msg, uk.pk);
    destroy_signed_message(msg);
}

// ok!
int test_create_and_destroy_hpoint()
{

    user_keys uk = create_key_pair();
    signed_message_t *msg1;
    msg1 = ls_get_a_signed_msg(uk); // generate random
    printf("message 1\n");
    DumpHex(msg1->message, msg1->length);
    validate_a_message(*msg1, uk.pk);

    user_keys uk2 = create_key_pair();
    signed_message_t *msg2;
    msg2 = ls_get_a_signed_msg(uk2); // generate random
    printf("message 2\n");
    DumpHex(msg2->message, msg2->length);
    validate_a_message(*msg2, uk2.pk);

    hash_point_p HASH_POINT = 0;
    HASH_POINT = create_hpoint_message(msg1, msg2);
    int rez = 0;
    if (HASH_POINT->hpoint1 == msg1)
        rez++;
    if (HASH_POINT->hpoint2 == msg2)
        rez++;
    destroy_signed_message(msg1);
    destroy_signed_message(msg2);
    destroy_hpoint_message(HASH_POINT);
    return (rez == 2) ? 0 : 1;
}
// OK
int test_process_messages_L1_v2()
{
    unsigned long long n_msg = (1LLU << 5LLU); //  create 2^9 messages
    user_keys uk = create_key_pair();
    signed_message_t **msg_arr = calloc(n_msg, sizeof(signed_message_t *));
    size_t i = 0;
    int rez = 0;
    for (i = 0; i < n_msg; i++)
    {
        msg_arr[i] = ls_get_a_signed_msg(uk); // pointer to message
        rez += validate_a_message(*msg_arr[i], uk.pk);
    }
    layer_hp *root_layer = process_s_messagesV2(n_msg, msg_arr);

    destoroy_a_layer(root_layer); // destroy level
    printf("rez is %d\n", rez);   // destoy created messages
    for (i = 0; i < n_msg; i++)
    {
        destroy_signed_message(msg_arr[i]);
    }
    free(msg_arr); // free conrainer for messages

    return (rez == n_msg) ? 0 : 1;
}

int test_create_tree()
{
    // create messages
    user_keys uk = create_key_pair();
    unsigned long long MSG_EXPONENT = 6LLU;
    layer_hp *L_arrays[MSG_EXPONENT];
    layer_hp L_arrays_p[MSG_EXPONENT]; // for free

    unsigned long long n_msg = (1LLU << MSG_EXPONENT); // level 0
    printf("N msg %llu\n", n_msg);

    L_arrays[MSG_EXPONENT - 1] = create_LEVEL0(&n_msg, uk); // need to free messages!
    L_arrays_p[MSG_EXPONENT - 1] = *L_arrays[MSG_EXPONENT - 1];

    fill_intermediate_levels(MSG_EXPONENT, &n_msg, L_arrays, L_arrays_p); // done
    /*
    size_t ii = 0;
    msg_link a_link;
    for (ii = 0; ii< L_arrays[MSG_EXPONENT-1]->size; ii++) {
        a_link = get_s_msg_from_L0( L_arrays[MSG_EXPONENT-1],ii );
        if (!(a_link.msg1 == (msg_arr_p)+ii)){
            break;
        } else if (!( a_link.msg2 == msg_arr_p+(ii + L_arrays[(MSG_EXPONENT-1)]->size ) )) {
            break;
        } else {
            continue;
        }
    }
    if (ii != 256) return 0;
    printf("ii is %lu\n",ii);
    */
    //---------------------------------------------
    // check others layer
    printf("getting msg from root node\n");
    DumpHex(get_a_hashes_Hnode(L_arrays, 0).Shash, crypto_generichash_BYTES);
    printf("\n");
    DumpHex(get_a_hashes_Hnode(L_arrays, 0).hash1, crypto_generichash_BYTES);
    printf("\n");
    DumpHex(get_a_hashes_Hnode(L_arrays, 0).hash2, crypto_generichash_BYTES);
    // check others layer
    printf("getting msg from root node\n");
    DumpHex(get_a_hashes_Hnode(L_arrays + 1, 0).Shash, crypto_generichash_BYTES);
    printf("\n");
    DumpHex(get_a_hashes_Hnode(L_arrays + 1, 0).hash1, crypto_generichash_BYTES);
    printf("\n");
    DumpHex(get_a_hashes_Hnode(L_arrays + 1, 0).hash2, crypto_generichash_BYTES);

    /*
    printf("root pointer\n");
    DumpHex( (*(L_arrays[0]->main_pointer))->hash, crypto_generichash_BYTES);
    printf("check hash from ouside side 1\n");
    DumpHex(  (((hash_point_p) ((*(L_arrays[0]->main_pointer))->hpoint1)))  , crypto_generichash_BYTES);
    printf("check hash from ouside side 2\n");
    DumpHex(  (((hash_point_p) ((*(L_arrays[0]->main_pointer))->hpoint2)))  , crypto_generichash_BYTES);
    */
    for (size_t i = 0; i < MSG_EXPONENT; i++)
    {
        destoroy_a_layer(L_arrays[i]);
    }
    /*
        for (size_t i =0; i< n_msg; i++) {
            free( msg_arr_p[i].message );
        }
    */
    //	layer_hp* L1 = create_a_h_layer(&n_msg,L0pointer);
}

int test_process_messages()
{

    user_keys uk = create_key_pair();
    unsigned long long n_msg = (1LLU << 9LLU);
    printf("n msg :%llu\n", n_msg);
    signed_message_t msg_arr[n_msg];
    for (size_t i = 0; i < n_msg; i++)
    {
        msg_arr[i] = *ls_get_a_signed_msg(uk); // generate random
        validate_a_message(msg_arr[i], uk.pk);
        DumpHex(msg_arr[i].message, msg_arr[i].length);
    }
    // create zero layer

    layer_hp *L0 = process_s_messages(n_msg, msg_arr);

    hash_point_p *L0pointer = L0->main_pointer;

    size_t msg_n_test = 0;
    msg_link a_link;
    get_s_msg_from_L0(L0, msg_n_test);

    size_t ii = 0;
    for (ii = 0; ii < L0->size; ii++)
    {

        a_link = get_s_msg_from_L0(L0, ii);
        if (!(a_link.msg1 == msg_arr + ii))
        {
            break;
        }
        else if (!(a_link.msg2 == msg_arr + (ii + L0->size)))
        {
            break;
        }
        else
        {
            continue;
        }
    }
    printf("ii is %lu\n", ii);
    destoroy_a_layer(L0);
    if (ii == L0->size)
        return 1;
    return 0;
}

int test_hashG_node_creation()
{
    // create 1 ground node with 2 msg
    user_keys uk = create_key_pair();
    signed_message_t *a_msg = get_a_signed_msg(uk);
    printf("message 1 is \n");
    DumpHex(a_msg->message, a_msg->length);
    printf("\n");

    signed_message_t *a_msg2 = get_a_signed_msg(uk);

    if (!validate_a_message(*a_msg, uk.pk))
        printf("validation error!\n");

    if (!validate_a_message(*a_msg2, uk.pk))
        printf("validation error!\n");

    hash_point_p aHP1 = create_hpoint_message(a_msg, a_msg2); // created
                                                              //

    // create 2 ground node with 2 msg
    signed_message_t *a_msg3 = get_a_signed_msg(uk);

    signed_message_t *a_msg4 = get_a_signed_msg(uk);

    if (!validate_a_message(*a_msg3, uk.pk))
        printf("validation error!\n");

    if (!validate_a_message(*a_msg4, uk.pk))
        printf("validation error!\n");

    hash_point_p aHP2 = create_hpoint_message(a_msg3, a_msg4); // created
                                                               //
    // crate G node
    hash_point_p GNODE = create_hpoint_hashG(aHP1, aHP2);
    printf("test message 1 from GNODE\n");
    if ((((hash_point_p)(GNODE->hpoint1))->messages.smsg_p1) == a_msg)
    {
        printf("validated msg1\n");
    }
    else
    {
        printf(" msg1 invalid");
    }
    if ((((hash_point_p)(GNODE->hpoint1))->messages.smsg_p2) == a_msg2)
    {
        printf("validated msg2\n");
    }
    else
    {
        printf(" msg2 invalid");
    }
    if ((((hash_point_p)(GNODE->hpoint2))->messages.smsg_p1) == a_msg3)
    {
        printf("validated msg3\n");
    }
    else
    {
        printf(" msg3 invalid");
    }
    if ((((hash_point_p)(GNODE->hpoint2))->messages.smsg_p2) == a_msg4)
    {
        printf("validated msg4\n");
    }
    else
    {
        printf(" msg4 invalid");
    }
    return 1;

    // DumpHex( ((hash_point_p) (GNODE->hpoint1))->messages.smsg_p1->message ,  ((hash_point_p)
    // (GNODE->hpoint1))->messages.smsg_p1->length ); printf("test message 2 from GNODE\n"); DumpHex( ((hash_point_p)
    // (GNODE->hpoint1))->messages.smsg_p1->message ,  ((hash_point_p) (GNODE->hpoint1))->messages.smsg_p1->length );

    return 1;
}

int test_hpoint_from_2msg_creation()
{

    user_keys uk = create_key_pair();
    signed_message_t *a_msg = get_a_signed_msg(uk);
    printf("message is\n");
    DumpHex(a_msg->message, a_msg->length);

    user_keys uk2 = create_key_pair();
    signed_message_t *a_msg2 = get_a_signed_msg(uk2);

    if (!validate_a_message(*a_msg, uk.pk))
        printf("validation error!\n");

    if (!validate_a_message(*a_msg2, uk2.pk))
        printf("validation error!\n");

    hash_point_p aHP1 = create_hpoint_message(a_msg, a_msg2);

    printf("check:\n");
    DumpHex(aHP1->messages.smsg_p1->message, aHP1->messages.smsg_p1->length);
    if ((a_msg) == aHP1->messages.smsg_p1)
    {
        printf("valid!\n");
        return 1;
    }
    else
    {
        return 0;
    }
}
/*
int test_hash_merging(){

    printf("TEST HASHIN MERGING\n");
        user_keys uk = create_key_pair();
        srand(time(NULL));
    test_msg_t somemsg = get_test_msg(100);
    signed_message_t a_msg = sign_a_message((unsigned char*)somemsg.test_msg,somemsg.len, uk.sk);


    user_keys uk2 = create_key_pair();
    test_msg_t somemsg2 = get_test_msg(100);
    signed_message_t a_msg2 = sign_a_message((unsigned char*)somemsg2.test_msg,somemsg2.len, uk2.sk);


    merge_2hashses(calc_hash(a_msg),calc_hash(a_msg2));
    return 1;
}
//test hash generating
int test_hash(){
    printf("TEST HASHING FUNCTION\n");
        user_keys uk = create_key_pair();
        srand(time(NULL));
    test_msg_t somemsg = get_test_msg(100);
    signed_message_t a_msg = sign_a_message((unsigned char*)somemsg.test_msg,somemsg.len, uk.sk);
    unsigned char hash[crypto_generichash_BYTES];
    unsigned char hash_re[crypto_generichash_BYTES];
    memcpy(hash,calc_hash(a_msg),crypto_generichash_BYTES);
    printf("calced hash:\n");
    DumpHex(hash,crypto_generichash_BYTES);
    size_t i = 0;
    for (i = 0; i< 500; i++) {
//		printf("i=%ld new hash:\n",i);
        memcpy(hash_re,calc_hash(a_msg),crypto_generichash_BYTES);
//		DumpHex(hash_re,crypto_generichash_BYTES);
        if (( strncmp((char*) hash ,(char*) hash_re ,crypto_generichash_BYTES ))!=0) break;

    }
    if (i==500) {
        printf("TEST PASSED, NO ERROR OCCURRED \n");
        return 0;
    }
    else printf("TEST PASSED, ERROR OCCURRED!!!!!!!!!!!!! \n");
    return 1;

}
//test message encryption
int test_valid_messages(){
        user_keys uk = create_key_pair();
    DumpHex(uk.sk,crypto_sign_SECRETKEYBYTES);
    DumpHex(uk.pk, crypto_sign_PUBLICKEYBYTES) ;
    printf("keys have been created\n");
    signed_message_t a_msg;
    //char* somemsg = "asdfghj";
        srand(time(NULL));
    size_t i = 0;
    for (i=0; i<15; i++) {
        int result = 0;
        test_msg_t somemsg = get_test_msg(15+i);
        //DumpHex(somemsg.test_msg, somemsg.len);
        for (size_t ii=0; ii<100; ii++) {
            a_msg = sign_a_message((unsigned char*)somemsg.test_msg,somemsg.len, uk.sk);
            //---validation
            ((validate_a_message(a_msg,uk.pk))) ? (result=1) : (result=0);
            if (!result) break;

        }
            free(somemsg.test_msg);
            free(a_msg.message);
            (result==1) ? printf("validated!\n") : printf("forged!\n");
            if (!result) break;
    }
    if (i==15) {
        printf("TEST PASSED, NO ERROR OCCURRED \n");
        return 0;
    }
    else printf("TEST PASSED, ERROR OCCURRED!!!!!!!!!!!!! \n");
    return 1;
}

//wrong keys
int test_INvalid_messages(){
        user_keys uk = create_key_pair();
        user_keys uk2 = create_key_pair();
    DumpHex(uk.sk,crypto_sign_SECRETKEYBYTES);
    DumpHex(uk.pk, crypto_sign_PUBLICKEYBYTES) ;
    printf("keys have been created\n");
    signed_message_t a_msg;
    //char* somemsg = "asdfghj";

        srand(time(NULL));
    test_msg_t somemsg[15];
    for (size_t k=0; k<15; k++) {
        somemsg[k] = get_test_msg(15);
    }

    int result = 0;
    for (size_t i=0; i<15; i++) {
            a_msg = sign_a_message((unsigned char*)somemsg[i].test_msg,somemsg[i].len, uk.sk);
            //---validation
            ((validate_a_message(a_msg,uk.pk))) ? (result+=1) : (result=0);
            free(somemsg[i].test_msg);
            free(a_msg.message);
            (result==1) ? printf("validated!\n") : printf("forged!\n");
    }
    if (result==15) {
        printf("TEST PASSED, NO ERROR OCCURRED \n");
        return 0;
    }
    else printf("TEST PASSED, ERROR OCCURRED!!!!!!!!!!!!! \n");
    return 1;
}
*/
