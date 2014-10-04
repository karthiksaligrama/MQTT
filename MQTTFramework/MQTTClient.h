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


typedef void (^MessageHandler)(MQTTMessage *message);

@interface MQTTClient : NSObject



-(void)setMessageRetryInterval: (NSUInteger)seconds;

-(MQTTClient *)initWithClientId:(NSString *)client;

-(void)connectWithHost:(NSString *)host;

@end

@protocol MQTTMessageDelegate <NSObject>



@end
