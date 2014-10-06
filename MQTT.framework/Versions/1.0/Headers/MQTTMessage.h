//
//  MQTTMessage.h
//  MQTTFramework
//
//  Created by Karthik Saligrama on 10/3/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum MessageQualityOfService:NSUInteger{
    AtMostOnce,
    AtLeastOnce,
    ExactlyOnce
} MessageQualityOfService;


@interface MQTTMessage : NSObject

@property(nonatomic,copy) NSString *topic;
@property(nonatomic,copy) NSData *payload;
@property MessageQualityOfService qos;
@property(nonatomic) NSNumber *messageId;

-(id)initWithTopic:(NSString *)topic
           payload:(NSData *)payload;

-(id)initWithTopic:(NSString *)topic
           payload:(NSData *)payload
               qos:(MessageQualityOfService)qos;

-(id)initWithTopic:(NSString *)topic
           payload:(NSData *)payload
               qos:(MessageQualityOfService)qos
               mid:(int)mid;

@end
