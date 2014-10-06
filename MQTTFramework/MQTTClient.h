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

typedef void (^MQTTConnectionCompletionHandler)(MQTTConnectionResponseCode responseCode);

@protocol MQTTMessageDelegate;

@interface MQTTClient : NSObject

@property(nonatomic,weak) MQTTConnectionCompletionHandler completionHandler;
@property(nonatomic,weak) id<MQTTMessageDelegate> delegate;

-(void)setMessageRetryInterval: (NSUInteger)seconds;
-(MQTTClient *)initWithClientId:(NSString *)client;
-(void)publishMessage:(MQTTMessage *)message;

-(void)connectWithHost:(NSString *)host;
-(void)connectWithHost:(NSString *)hostName withPort:(int)port enableSSL:(bool)ssl;
-(void)connectWithHost:(NSString *)hostName withSSL:(BOOL)ssl;

@end



@protocol MQTTMessageDelegate <NSObject>

@optional
-(void)onMessageRecieved:(MQTTMessage *)message;
-(void)onSubscribeWithClient:(MQTTClient *)client;
-(void)onPublishWithClient:(MQTTClient *)client;
-(void)onDisconnect:(MQTTConnectionResponseCode)rc;
@end