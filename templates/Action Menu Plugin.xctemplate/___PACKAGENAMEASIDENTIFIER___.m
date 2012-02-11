//
//  ___FILENAME___
//  ___PACKAGENAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//  Copyright (c) ___YEAR___ ___ORGANIZATIONNAME___. All rights reserved.
//

// Action Menu by Ryan Petrich

#import <UIKit/UIKit.h>
#import "ActionMenu.h"

@implementation UIResponder (___PROJECTNAMEASIDENTIFIER___Action)

- (void)do___PROJECTNAMEASIDENTIFIER___:(id)sender
{
	// TODO: Implement ___PROJECTNAME___ Plugin
}

- (BOOL)canDo___PROJECTNAMEASIDENTIFIER___:(id)sender
{
	return YES;
}

+ (void)load
{
	[[UIMenuController sharedMenuController] registerAction:@selector(do___PROJECTNAMEASIDENTIFIER___:) title:@"___PROJECTNAME___" canPerform:@selector(canDo___PROJECTNAMEASIDENTIFIER___:)];
}

@end
