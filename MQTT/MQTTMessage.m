//
//  MQTTMessage.m
//  MQTTFramework
//
//  Created by Karthik Saligrama on 10/3/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import "MQTTMessage.h"

@implementation MQTTMessage

-(id)initWithTopic:(NSString *)topic
           payload:(NSData *)payload
{
    self = [self initWithTopic:topic payload:payload qos:3];
    return self;
}

-(id)initWithTopic:(NSString *)topic
           payload:(NSData *)payload
               qos:(MessageQualityOfService)qos
{
    self = [self init];
    if(self){
        self.topic = topic;
        self.payload = payload;
        self.qos =qos;
    }
    return self;
}

-(id)initWithTopic:(NSString *)topic
           payload:(NSData *)payload
               qos:(MessageQualityOfService)qos
               mid:(int)mid
{
    self = [self initWithTopic:topic payload:payload qos:qos];
    if(self){
        self.messageId = [NSNumber numberWithInt:mid];
    }
    return self;
}

@end
