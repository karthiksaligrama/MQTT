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
-(void)connectWithHost:(NSString *)host;

-(void)publishMessage:(MQTTMessage *)message;

@end



@protocol MQTTMessageDelegate <NSObject>
@required
-(void)onMessageRecieved:(MQTTMessage *)message;

@end