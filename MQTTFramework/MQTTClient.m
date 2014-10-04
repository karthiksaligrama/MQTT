//
//  MQTTClient.m
//  MQTTClient
//
//  Created by Karthik Saligrama on 10/3/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import "MQTTClient.h"
#import "mosquitto.h"

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
        self.clientId = clientId;
        
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
        
        self.queue = dispatch_queue_create(client_id, nil);

    }
    return self;
}

-(void)setMessageRetryInterval: (NSUInteger)seconds{
    mosquitto_message_retry_set(mosq, (unsigned int)seconds);
}

#pragma mark - connection


-(void)connectWithHost:(NSString *)hostName{
    [self connectWithHost:hostName withSSL:NO];
}

-(void)connectWithHost:(NSString *)hostName withSSL:(BOOL)ssl{
    self.host = hostName;
    self.sslEnabled = ssl;
    self.port = self.sslEnabled?8883:1883;
    
    const char *cstrHost = [self.host cStringUsingEncoding:NSASCIIStringEncoding];
    mosquitto_username_pw_set(mosq, NULL , NULL);
    mosquitto_reconnect_delay_set(mosq, self.reconnectDelay, self.reconnectDelayMax, self.reconnectExponentialBackoff);
    mosquitto_connect(mosq, cstrHost, self.port, self.keepAlive);
    
    dispatch_async(self.queue, ^{
        mosquitto_loop_forever(mosq, -1, 1);
    });
}

#pragma mark - Publishing part

-(void)publishMessage:(MQTTMessage *)message oncomplete:(MessageHandler)handler{
    
}


#pragma mark - Subscribing part




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
    
}

void on_disconnect_callback(struct mosquitto *mosq, void *obj,int rc){
    MQTTClient *client = (__bridge MQTTClient *)obj;
    client.isConnected = NO;
}

void on_publish_callback(struct mosquitto *mosq, void *obj, int mid){
    
}

void on_subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos){
    
}

void on_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message){
    
}

void on_unsubscribe_callback(struct mosquitto *mosq, void *obj, int mid){
    
}

void on_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str){
    
}


@end
