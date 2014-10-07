//
//  MQTTClient.h
//  MQTTClient
//
//  Created by Karthik Saligrama on 10/3/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MQTTMessage.h"



typedef enum MQTTConnectionResponse:NSUInteger{
    ConnectionAccepted,
    ProtocolViolation,
    IdentifierRejected,
    ServerUnavailable,
    ConnectionRefused
} MQTTConnectionResponseCode;

typedef void (^MQTTSubscribeHandler)(NSArray *qosGranted);

@protocol MQTTMessageDelegate;

@interface MQTTClient : NSObject

@property(nonatomic,weak) id<MQTTMessageDelegate> delegate;

/*
 *  Initialize the MQTT Client
 */

-(MQTTClient *)initWithClientId:(NSString *)client;

/*
 * Connect with Hostname
 * This connection with server is not secure. It connects to server with default port 1883.
 */
-(void)connectWithHost:(NSString *)hostName;

/*
 * Connect with host name and ssl flag
 * If SSL Flag is set then it connects to the default port 8883, if not then it connects to 1883 (non ssl)
 */
-(void)connectWithHost:(NSString *)hostName withSSL:(BOOL)ssl;

/*
 * Connect with remote server with host name, ssl flag & port number.
 * Use this if you want to explicitly specify the port number in your 
 * application incase you are not using standard ports
 */
-(void)connectWithHost:(NSString *)hostName withPort:(int)port enableSSL:(bool)ssl;

/*
 * Publish message to the MQTT Server.
 * return the message id of the published message.
 * Store it and retrieve handle it in the callback.
 */
-(NSNumber *)publishMessage:(MQTTMessage *)message;


/*
 * Set the message retry interval in case of publishing the message
 */
-(void)setMessageRetryInterval: (NSUInteger)seconds;

/*
 * Subscribe Message from the MQTT server with a given topic and quality of service
 */

-(void)subscribeToTopic:(NSString *)topic qos:(MessageQualityOfService)qos subscribeHandler:(MQTTSubscribeHandler)handler;

/*
 * Unsubscribe to topic from the MQTT server
 */

-(void)unsubscribeToTopic:(NSString *)topic;


@end



@protocol MQTTMessageDelegate <NSObject>

@optional
-(void)onMessageRecieved:(MQTTMessage *)message;
-(void)onPublishMessage:(NSNumber *)messageId;
-(void)onConnected:(MQTTConnectionResponseCode)rc;
-(void)onDisconnect:(MQTTConnectionResponseCode)rc;
@end