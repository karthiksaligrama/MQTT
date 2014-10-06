//
//  sslhelper.c
//  MQTTFramework
//
//  Created by Karthik Saligrama on 10/5/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//  class helps in fetching the ssl certificate
//

#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

/* ---------------------------------------------------------- *
 * First we need to make a standard TCP socket connection.    *
 * create_socket() creates a socket & TCP-connects to server. *
 * ---------------------------------------------------------- */
int create_socket(char[], BIO *);

int save_ssl_certificate_at_path(char dest_url[], const char *filename)
{
    BIO              *certbio = NULL;
    BIO               *outbio = NULL;
    X509                *cert = NULL;
    X509_NAME       *certname = NULL;
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL *ssl;
    int server = 0;
    int ret, i;
    
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    
    certbio = BIO_new(BIO_s_file());
    outbio  = BIO_new_file(filename, "w");
    
    //initialize SSL library and register algorithms
    if(SSL_library_init() < 0){
        raise(SIGINT);
    }
    
    method = SSLv23_client_method();
    
    //Try to create a new SSL context
    if ( (ctx = SSL_CTX_new(method)) == NULL){
        raise(SIGINT);
    }
    
    //Disabling SSLv2 will leave v3 and TSLv1 for negotiation
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
    
    //Create new SSL connection state object
    ssl = SSL_new(ctx);
    
    //Make the underlying TCP socket connection
    server = create_socket(dest_url, outbio);
    
    //Attach the SSL session to the socket descriptor
    SSL_set_fd(ssl, server);
    
    //Try to SSL-connect here, returns 1 for success
    SSL_connect(ssl);
    
    //Get the remote certificate into the X509 structure
    cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL)//{
        raise(SIGINT);
    
    //print the certificate
    PEM_write_bio_X509(outbio, cert);
    
    //free up the memory
    SSL_free(ssl);
    close(server);
    X509_free(cert);
    SSL_CTX_free(ctx);
    return(0);
}

/* ---------------------------------------------------------- *
 * create_socket() creates the socket & TCP-connect to server *
 * ---------------------------------------------------------- */
int create_socket(char url_str[], BIO *out) {
    int sockfd;
    char hostname[256] = "";
    char    portnum[6] = "443";
    char      proto[6] = "";
    char      *tmp_ptr = NULL;
    int           port;
    struct hostent *host;
    struct sockaddr_in dest_addr;
    
    /* ---------------------------------------------------------- *
     * Remove the final / from url_str, if there is one           *
     * ---------------------------------------------------------- */
    if(url_str[strlen(url_str)] == '/')
        url_str[strlen(url_str)] = '\0';
    
    /* ---------------------------------------------------------- *
     * the first : ends the protocol string, i.e. http            *
     * ---------------------------------------------------------- */
    strncpy(proto, url_str, (strchr(url_str, ':')-url_str));
    
    /* ---------------------------------------------------------- *
     * the hostname starts after the "://" part                   *
     * ---------------------------------------------------------- */
    strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));
    
    /* ---------------------------------------------------------- *
     * if the hostname contains a colon :, we got a port number   *
     * ---------------------------------------------------------- */
    if(strchr(hostname, ':')) {
        tmp_ptr = strchr(hostname, ':');
        /* the last : starts the port number, if avail, i.e. 8443 */
        strncpy(portnum, tmp_ptr+1,  sizeof(portnum));
        *tmp_ptr = '\0';
    }
    
    port = atoi(portnum);
    
    if ( (host = gethostbyname(hostname)) == NULL ) {
        abort();
    }
    
    /* ---------------------------------------------------------- *
     * create the basic TCP socket                                *
     * ---------------------------------------------------------- */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port=htons(port);
    dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);
    
    /* ---------------------------------------------------------- *
     * Zeroing the rest of the struct                             *
     * ---------------------------------------------------------- */
    memset(&(dest_addr.sin_zero), '\0', 8);
    
    tmp_ptr = inet_ntoa(dest_addr.sin_addr);
    
    /* ---------------------------------------------------------- *
     * Try to make the host connect here                          *
     * ---------------------------------------------------------- */
    if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                 sizeof(struct sockaddr)) == -1 ) {
        //do nothing
        //log information
    }
    
    return sockfd;
}


/* for testing
 int main()
 {
 return save_ssl_certificate_at_path("https://nextstep.tcs.com:443","/Users/kass/Desktop/filename.crt");
 }
 */