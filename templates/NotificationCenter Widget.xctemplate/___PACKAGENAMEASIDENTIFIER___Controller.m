//
//  ___FILENAME___
//  ___PACKAGENAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//  Copyright (c) ___YEAR___ ___ORGANIZATIONNAME___. All rights reserved.
//

#import "___FILEBASENAME___.h"

@implementation ___FILEBASENAMEASIDENTIFIER___

-(id)init
{
	if ((self = [super init]))
	{
	}

	return self;
}

-(void)dealloc
{
	[_view release];
	[super dealloc];
}

- (UIView *)view
{
	if (_view == nil)
	{
		_view = [[UIView alloc] initWithFrame:CGRectMake(2, 0, 316, 71)];

		UIImage *bg = [[UIImage imageWithContentsOfFile:@"/System/Library/WeeAppPlugins/___PACKAGENAME___.bundle/WeeAppBackground.png"] stretchableImageWithLeftCapWidth:5 topCapHeight:71];
		UIImageView *bgView = [[UIImageView alloc] initWithImage:bg];
		bgView.frame = CGRectMake(0, 0, 316, 71);
		[_view addSubview:bgView];
		[bgView release];

		UILabel *lbl = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 316, 71)];
		lbl.backgroundColor = [UIColor clearColor];
		lbl.textColor = [UIColor whiteColor];
		lbl.text = @"Hello, World!";
		lbl.textAlignment = UITextAlignmentCenter;
		[_view addSubview:lbl];
		[lbl release];
	}

	return _view;
}

- (float)viewHeight
{
	return 71.0f;
}

@end