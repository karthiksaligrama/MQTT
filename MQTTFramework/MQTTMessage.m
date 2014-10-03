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
    self = [self init];
    if(self){
        self.topic = topic;
        self.payload = payload;
    }
    return self;
}



@end
