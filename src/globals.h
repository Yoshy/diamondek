// (C) 2011 PowerBIT Studio
/*! 
    \brief Global definitions
*/
//#undef DEBUG_RENDER
#define DEBUG_RENDER

#define RESOLUTION_X 800
#define RESOLUTION_Y 600

#define BALL_SIZE 15

#define PADDLE_WIDTH 134
#define PADDLE_HEIGHT 28
#define PADDLE_POS_X 350
#define PADDLE_POS_Y 550

#define LIVES_MAX 3
#define LIVES_TEXT_X 710
#define LIVES_TEXT_Y 6
#define GEMS_TEXT_X 604
#define GEMS_TEXT_Y 6
#define LEVEL_INFO_TEXT_X 50
#define LEVEL_INFO_TEXT_Y 6
#define PAUSED_FONT_SIZE 100

// Update 300 times per second
#define UPDATE_PERIOD_MSEC (1000 / 300)
// Speed defined in pixels per update period
#define PADDLE_SPEED 4.0f
#define BALL_SPEED 1.4f
#define BALL_SPEED_X 0.0f
#define BALL_SPEED_Y -BALL_SPEED

// G-force, applied to some objects (diamonds for example)
#define G_ACCELERATION 0.001f

// Damping ratio
#define DAMPING_RATIO_SQUARE 0.2f

// Explosion parameters
#define EXPLOSION_RADIUS 150
#define EXPLOSION_MAX_DISTORTION 5