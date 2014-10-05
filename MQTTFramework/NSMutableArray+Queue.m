//
//  NSMutableArray+Queue.m
//  MQTTFramework
//
//  Created by Karthik Saligrama on 10/5/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import "NSMutableArray+Queue.h"

@implementation NSMutableArray (Queue)

-(id)dequeue{
    id head = [self objectAtIndex:0];
    if(head!=nil){
        [self removeObjectAtIndex:0];
    }
    return head;
}

-(id)peek{
    id head = [self objectAtIndex:0];
    return head;
}

-(void)enqueue:(id)object{
    [self addObject:object];
}

@end
