//
//  MQTTMessage.h
//  MQTTFramework
//
//  Created by Karthik Saligrama on 10/3/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MQTTMessage : NSObject

@property(nonatomic,copy) NSString *topic;
@property(nonatomic,copy) NSData *payload;

-(id)initWithTopic:(NSString *)topic
           payload:(NSData *)payload;

@end
