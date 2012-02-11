@protocol BBWeeAppController <NSObject>
- (id)view;

@optional
- (float)viewHeight;
- (void)viewWillAppear;
- (void)viewDidAppear;
- (void)viewWillDisappear;
- (void)viewDidDisappear;
- (void)willRotateToInterfaceOrientation:(int)arg1;
- (void)willAnimateRotationToInterfaceOrientation:(int)arg1;
- (void)didRotateFromInterfaceOrientation:(int)arg1;
- (void)loadPlaceholderView;
- (void)loadFullView;
- (void)unloadView;
- (void)loadView;
- (id)launchURL;
- (id)launchURLForTapLocation:(struct CGPoint)arg1;
@end