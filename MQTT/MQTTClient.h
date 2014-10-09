//
//  MQTTClient.h
//  MQTTClient
//
//  Created by Karthik Saligrama on 10/3/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MQTTMessage.h"

#define CERT_FILE @"CERT_FILE"
#define KEY_FILE @"KEY_FILE"
#define CA_PATH @"CA_PATH"
#define CA_FILE @"CA_FILE"

typedef enum MQTTConnectionResponse:NSUInteger{
    ConnectionAccepted,
    ProtocolViolation,
    IdentifierRejected,
    ServerUnavailable,
    ConnectionRefused
} MQTTConnectionResponseCode;

typedef void (^MQTTSubscribeHandler)(NSArray *qosGranted);
typedef NSString* (^PasswordCallback)();

@protocol MQTTMessageDelegate;

@interface MQTTClient : NSObject

@property(nonatomic,weak) id<MQTTMessageDelegate> delegate;

/*
 *  Initialize the MQTT Client
 */
-(MQTTClient *)initWithClientId:(NSString *)client;

/*
 * Set the username and password for the mosquitto broker to connect
 * Call this before calling connect method incase you are using username and password
 *
 */
-(void)setUsername:(NSString *)username Password:(NSString *)password;

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
 * Connect with remote server with host name, ssl flag , port number and certificate file.
 * Useful when you are not using trusted CA or Self Signed CA
 */
-(void)connectWithHost:(NSString *)hostName withPort:(int)port enableSSL:(bool)ssl usingSSLCACert:(NSString *)certFile;

/*
 * Incase you are using self signed certificates. 
 * #warning Donot use in production.
 * call before using connect and after calling setSSLSettings method
 */
-(void)setSSLInsecure:(BOOL)insecure;

/*
 * Settings for the SSL.
 * Accepts a dictionary with the following values;
 * CA_PATH,CA_FILE,CERT_FILE,KEY_FILE
 * Set the passwordCallback incase the keyfile is encrypted.
 * Needs to be called before you call the connect method.
 */
-(void)setSSLSettings:(NSDictionary *)options passwordCallback:(PasswordCallback) pwdCallback;

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

/*
 * Disconnect from Server
 */
-(void)disconnect;


@end

@protocol MQTTMessageDelegate <NSObject>

@optional
-(void)onMessageRecieved:(MQTTMessage *)message;
-(void)onPublishMessage:(NSNumber *)messageId;
-(void)onConnected:(MQTTConnectionResponseCode)rc;
-(void)onDisconnect:(MQTTConnectionResponseCode)rc;
@end