//
//  MQTTClient.m
//  MQTTClient
//
//  Created by Karthik Saligrama on 10/3/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import "MQTTClient.h"
#import "mosquitto.h"
#import <openssl/ssl.h>
#import <openssl/err.h>
#import "sslhelper.h"


#define SSL_CERTIFICATE_PATH @"/tmp/ssl_certificate.crt"

@interface MQTTClient()

@property(nonatomic,assign) BOOL isConnected;
@property(nonatomic,copy) NSString *host;
@property(nonatomic,copy) NSString *clientId;
@property(nonatomic,assign) BOOL sslEnabled;
@property(nonatomic,assign) unsigned int port;
@property(nonatomic,assign) unsigned int keepAlive;
@property(nonatomic,assign) unsigned int reconnectDelay;
@property(nonatomic,assign) unsigned int reconnectDelayMax;
@property(nonatomic,assign) unsigned int reconnectExponentialBackoff;
@property(nonatomic,assign) unsigned int maxInflightMessage;
@property(nonatomic,strong) dispatch_queue_t queue;


//queues for holding the data that is to be published
@property(nonatomic,strong) NSMutableDictionary *publishQueue;
@property(nonatomic,strong) NSMutableDictionary *subscribeQueue;

@end

@implementation MQTTClient

struct mosquitto *mosq;

@synthesize host;
@synthesize clientId;
@synthesize isConnected;

-(MQTTClient *)initWithClientId:(NSString *)client{
    self = [super init];
    if(self){
        mosquitto_lib_init();
        self.clientId = client;
        
        self.keepAlive = 120;
        self.reconnectDelay = 1;
        self.reconnectDelayMax = 1;
        self.reconnectExponentialBackoff = NO;
        self.maxInflightMessage = 100;
        if(!clientId)
            [NSException raise:@"No Client Id" format:@"Mqtt Client is not intantiated with client id"];
        
        const char *client_id = [self.clientId cStringUsingEncoding:NSUTF8StringEncoding];
        
        mosq = mosquitto_new(client_id, NO, (__bridge void *)(self));
        mosquitto_connect_callback_set(mosq, on_connect_callback);
        mosquitto_disconnect_callback_set(mosq,on_disconnect_callback);
        mosquitto_publish_callback_set(mosq,on_publish_callback);
        mosquitto_subscribe_callback_set(mosq,on_subscribe_callback);
        mosquitto_message_callback_set(mosq,on_message_callback);
        mosquitto_unsubscribe_callback_set(mosq,on_unsubscribe_callback);
        mosquitto_log_callback_set(mosq,on_log_callback);
        mosquitto_reconnect_delay_set(mosq,self.reconnectDelay,self.reconnectDelayMax,self.reconnectExponentialBackoff);
        mosquitto_max_inflight_messages_set(mosq,self.maxInflightMessage);

        self.publishQueue = [[NSMutableDictionary alloc]init];
        self.subscribeQueue = [[NSMutableDictionary alloc]init];
        
        self.queue = dispatch_queue_create(client_id, nil);

    }
    return self;
}

#pragma mark - connection

-(void)connectWithHost:(NSString *)hostName{
    [self connectWithHost:hostName withSSL:NO];
}

-(void)connectWithHost:(NSString *)hostName withPort:(int)port enableSSL:(bool)ssl{
    self.host = hostName;
    self.sslEnabled = ssl;
    self.port = port;
    
    if(self.sslEnabled){
        //save the certificate for ssl connection
        
        const char *caFilePath = [SSL_CERTIFICATE_PATH cStringUsingEncoding:NSUTF8StringEncoding];
        
        //TODO: validate if the certificate is already present
        //if certificate is already present then we can validate the certificate
        //if validation fails trigger saving of the certificate again
        NSString *url = [[NSString alloc]initWithFormat:@"ssl://%@:%d",hostName,self.port];
        const char *c = [url UTF8String];
        char *cpy = calloc([url length]+1, 1);
        strncpy(cpy, c, [url length]);
        save_ssl_certificate_at_path(cpy,caFilePath);
        free(cpy);
        
        mosquitto_tls_set(mosq,caFilePath, NULL, NULL, NULL, NULL);
    }
    
    const char *cstrHost = [self.host cStringUsingEncoding:NSASCIIStringEncoding];
    mosquitto_username_pw_set(mosq, NULL , NULL);
    mosquitto_reconnect_delay_set(mosq, self.reconnectDelay, self.reconnectDelayMax, self.reconnectExponentialBackoff);
    mosquitto_connect(mosq, cstrHost, self.port, self.keepAlive);
    
    dispatch_async(self.queue, ^{
        mosquitto_loop_forever(mosq, -1, 1);
    });
}

-(void)connectWithHost:(NSString *)hostName withSSL:(BOOL)ssl{
    int p = ssl?8883:1883;
    [self connectWithHost:hostName withPort:p enableSSL:ssl];
}


#pragma mark - Publishing part

-(NSNumber *)publishMessage:(MQTTMessage *)message{
    const char* topic = [message.topic cStringUsingEncoding:NSUTF8StringEncoding];
    int mid;
    [self.publishQueue setObject:message forKey:[NSNumber numberWithInt:mid]];
    mosquitto_publish(mosq, &mid, topic, (int)message.payload.length, message.payload.bytes, message.qos, NO);
    [message setMessageId:[NSNumber numberWithInt:mid]];
    return [message messageId];
}


-(void)setMessageRetryInterval: (NSUInteger)seconds{
    mosquitto_message_retry_set(mosq, (unsigned int)seconds);
}

#pragma mark - Subscribing part
-(void)subscribeToTopic:(NSString *)topic qos:(MessageQualityOfService)qos subscribeHandler:(MQTTSubscribeHandler)handler{
    const char* cStringTopic = [topic cStringUsingEncoding:NSUTF8StringEncoding];
    int mid;
    mosquitto_subscribe(mosq, &mid, cStringTopic, qos);
    if(handler)
        [self.subscribeQueue setObject:handler forKey:[NSNumber numberWithInt:mid]];
}

-(void)unsubscribeToTopic:(NSString *)topic{
    const char* cstringTopic = [topic cStringUsingEncoding:NSUTF8StringEncoding];
    int mid;
    mosquitto_unsubscribe(mosq, &mid, cstringTopic);
    [self.subscribeQueue removeObjectForKey:[NSNumber numberWithInt:mid]];
}

#pragma mark - dealloc
-(void)dealloc{
    if(mosq){
        mosquitto_destroy(mosq);
        mosq = NULL;
    }
}

#pragma mark - Callback methods from libmosquitto

void on_connect_callback(struct mosquitto *mosq, void *obj, int rc){
    MQTTClient *client = (__bridge MQTTClient *)obj;
    client.isConnected = (rc == ConnectionAccepted);
    if(client.delegate && [client.delegate respondsToSelector:@selector(onConnected:)]){
        [client.delegate onConnected:rc];
    }
}

void on_disconnect_callback(struct mosquitto *mosq, void *obj,int rc){
    MQTTClient *client = (__bridge MQTTClient *)obj;
    client.isConnected = NO;
    [client.publishQueue removeAllObjects];
    if(client.delegate && [client.delegate respondsToSelector:@selector(onDisconnect:)]){
        [client.delegate onDisconnect:rc];
    }
}

void on_publish_callback(struct mosquitto *mosq, void *obj, int mid){
    MQTTClient *client = (__bridge MQTTClient *)(obj);
    if(client.delegate && [client.delegate respondsToSelector:@selector(onPublishMessage:)]){
        [client.delegate onPublishMessage:[NSNumber numberWithInt:mid]];
        [client.publishQueue removeObjectForKey:[NSNumber numberWithInt:mid]];
    }
}

void on_subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos){
    MQTTClient *client = (__bridge MQTTClient *)(obj);
    MQTTSubscribeHandler handler = [client.subscribeQueue objectForKey:[NSNumber numberWithInt:mid]];
    if(handler){
        NSMutableArray *qosGranted = [NSMutableArray arrayWithCapacity:qos_count];
        for (int i = 0; i < qos_count; i++) {
            [qosGranted addObject:[NSNumber numberWithInt:granted_qos[i]]];
        }
        handler(qosGranted);
        [client.subscribeQueue removeObjectForKey:[NSNumber numberWithInt:mid]];
    }
}

void on_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *mosquitto_message){
    NSString *topic = [NSString stringWithUTF8String: mosquitto_message->topic];
    NSData *payload = [NSData dataWithBytes:mosquitto_message->payload length:mosquitto_message->payloadlen];
    
    MQTTMessage *message = [[MQTTMessage alloc]initWithTopic:topic payload:payload qos:mosquitto_message->qos];
    
    MQTTClient* client = (__bridge MQTTClient*)obj;
    if(client.delegate && [client.delegate respondsToSelector:@selector(onMessageRecieved:)]){
        [client.delegate onMessageRecieved:message];
    }
}

void on_unsubscribe_callback(struct mosquitto *mosq, void *obj, int mid){
    MQTTClient *client = (__bridge MQTTClient *)(obj);
    
}

void on_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str){
    
}

@end
