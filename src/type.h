/*
 * BSD 2-Clause License
 * Copyright (c) 2024, Yazeed Alharthi
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *	  1. Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer.
 *
 *	  2. Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in the
 *	  documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ZWM_TYPE_H
#define ZWM_TYPE_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <xcb/randr.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

#define CAP					 3
#define W_GAP				 10
#define MAXLEN				 (2 << 7)
#define DLEN				 (2 << 4)
#define NULL_STR			 "N/A"
#define MONITOR_NAME		 "DEF_MONITOR"
#define ROOT_WINDOW			 "root ZWM"
#define NORMAL_BORDER_COLOR	 0x30302f
#define ACTIVE_BORDER_COLOR	 0x83a598
#define BORDER_WIDTH		 2
#define FOCUS_FOLLOW_POINTER true

typedef xcb_connection_t xcb_conn_t;

typedef enum {
	HORIZONTAL_TYPE,
	VERTICAL_TYPE,
	DYNAMIC_TYPE
} split_type_t;

typedef enum {
	HORIZONTAL_FLIP,
	VERTICAL_FLIP
} flip_t;

typedef enum {
	GROW = 1 << 1,
	SHRINK
} resize_t;

typedef enum {
	LEFT = 1 << 1,
	RIGHT,
	UP,
	DOWN,
	NONE
} direction_t;

typedef enum {
	ERROR,
	INFO,
	DEBUG,
	WARNING
} log_level_t;

typedef enum {
	CURSOR_POINTER = 0,
	CURSOR_WATCH,
	CURSOR_MOVE,
	CURSOR_XTERM,
	CURSOR_NOT_ALLOWED,
	CURSOR_HAND2,
	CURSOR_MAX
} cursor_t;

/* bit flags to detrmine the change in monitors' state */
typedef enum {
	_NONE		 = 0x00,
	CONNECTED	 = 0x01,
	DISCONNECTED = 0x02,
	/* LAYOUT includes resolution, position, or orientation changes */
	LAYOUT		 = 0x04
} monitor_state_t;

typedef struct {
	// 2^16 = 65535
	uint16_t previous_x, previous_y;
	uint16_t current_x, current_y;
} posxy_t;

typedef struct {
	int16_t	 x;
	int16_t	 y;
	uint16_t width;
	uint16_t height;
} rectangle_t;

typedef enum {
	TILED,
	FLOATING,
	FULLSCREEN
} state_t;

typedef enum {
	DEFAULT = 1 << 1,
	MASTER,
	STACK,
	GRID
} layout_t;

typedef struct {
	uint32_t	 border_width;
	xcb_window_t window;
	xcb_atom_t	 type;
	state_t		 state;
} client_t;

typedef enum {
	ROOT_NODE = 1 << 1,
	INTERNAL_NODE,
	EXTERNAL_NODE
} node_type_t;

typedef struct node_t node_t;
struct node_t {
	node_t	   *parent;
	node_t	   *first_child;
	node_t	   *second_child;
	client_t   *client;
	node_type_t node_type;
	rectangle_t rectangle;
	rectangle_t floating_rectangle;
	bool		is_focused;
	bool		is_master;
};

typedef struct {
	node_t	*tree;
	char	 name[DLEN];
	uint8_t	 id;
	uint8_t	 n_count;
	layout_t layout;
	bool	 is_focused;
} desktop_t;

typedef struct monitor_t monitor_t;
struct monitor_t {
	desktop_t		 **desktops;
	monitor_t		  *next;
	char			   name[DLEN];
	uint32_t		   id;
	xcb_randr_output_t randr_id;
	xcb_window_t	   root;
	rectangle_t		   rectangle;
	bool			   is_wired;
	bool			   is_focused;
	bool			   is_occupied;
	bool			   is_primary;
	uint8_t			   n_of_desktops;
};

typedef struct {
	uint32_t	 id;
	xcb_window_t window;
	rectangle_t	 rectangle;
} bar_t;

typedef struct {
	xcb_connection_t	  *connection;
	xcb_ewmh_connection_t *ewmh;
	xcb_screen_t		  *screen;
	bar_t				  *bar;
	xcb_window_t		   root_window;
	split_type_t		   split_type;
	uint8_t				   screen_nbr;
} wm_t;

typedef struct {
	char	  **cmd;
	uint8_t		argc;
	uint8_t		idx;
	resize_t	r;
	layout_t	t;
	direction_t d;
	state_t		s;
} arg_t;

typedef struct {
	uint32_t	 mod;
	xcb_keysym_t keysym;
	int (*function_ptr)(arg_t *);
	arg_t *arg;
} _key__t;

typedef struct conf_key_t conf_key_t;
struct conf_key_t {
	uint32_t	 mod;
	xcb_keysym_t keysym;
	int (*function_ptr)(arg_t *);
	arg_t	   *arg;
	conf_key_t *next;
};

typedef struct {
	char *func_name;
	int (*function_ptr)(arg_t *);
} conf_mapper_t;

typedef struct {
	char		 key[6];
	xcb_keysym_t keysym;
} key_mapper_t;

typedef struct {
	uint16_t border_width;
	uint16_t window_gap;
	uint32_t active_border_color;
	uint32_t normal_border_color;
	int		 virtual_desktops;
	bool	 focus_follow_pointer;
} config_t;

typedef struct rule_t rule_t;
struct rule_t {
	char	win_name[256];
	state_t state;
	int		desktop_id;
	rule_t *next;
};

typedef struct queue_node_t queue_node_t;
struct queue_node_t {
	node_t		 *tree_node;
	queue_node_t *next;
};

typedef struct {
	queue_node_t *front;
	queue_node_t *rear;
} queue_t;

#endif // ZWM_TYPE_H
