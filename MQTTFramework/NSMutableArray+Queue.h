//
//  NSMutableArray+Queue.h
//  MQTTFramework
//
//  Created by Karthik Saligrama on 10/5/14.
//  Copyright (c) 2014 Karthik Saligrama. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSMutableArray (Queue)

-(id)dequeue;

-(void)enqueue:(id)object;

-(id)peek;

@end
