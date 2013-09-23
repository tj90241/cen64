/* ============================================================================
 *  CEN64.c: Main application.
 *
 *  CEN64: Cycle-accurate, Efficient Nintendo 64 Simulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "CEN64.h"
#include "Device.h"

#ifdef __cplusplus
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#else
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <GL/glfw.h>

/* GLFW seems to like global state. */
/* We'll jmp back into main at close. */
static jmp_buf env;

/* ============================================================================
 *  CloseRequested: GLFW requested a close; jump to saved environment.
 * ========================================================================= */
static int
CloseRequested(void) {
  longjmp(env, 1);
  return 0;
}

/* ============================================================================
 *  ParseArgs: Parses the argument list and performs actions.
 * ========================================================================= */
static void
ParseArgs(int args, const char *argv[], struct CEN64Device *device) {
  int i;

  /* TODO: getops or something sensible. */
  for (i = 0; i < args; i++) {
    const char *arg = argv[i];

    while (*arg == ' ');

    /* Accept -, --, and / */
    if (*arg == '-') {
      arg++;

      if (*arg == '-')
        arg++;
    }

    else if (*arg == '/')
      arg++;

    /* Set backing EEPROM file. */
    if (!strcmp(arg, "eeprom")) {
      if (++i >= args) {
        printf("-eeprom: Missing argument; ignoring.\n");
        continue;
      }

      SetEEPROMFile(device->pif, argv[i]);
    }
  }
}

/* ============================================================================
 *  main: Parses arguments and kicks off the application.
 * ========================================================================= */
int main(int argc, const char *argv[]) {
  struct CEN64Device *device;

  if (argc < 3) {
    printf(
      "Usage: %s <pifrom> <cart> [options]\n\n"
      "Options:\n"
      "  -eeprom <file>\n\n",
      argv[0]);

    printf("RSP Build Type: %s\nRDP Build Type: %s\n",
      RSPBuildType, RDPBuildType);

    return 0;
  }

  if (glfwInit() != GL_TRUE) {
    printf("Failed to initialize GLFW.\n");
    return 255;
  }

  glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
  if (glfwOpenWindow(640, 480, 5, 6, 5, 0, 8, 0, GLFW_WINDOW) != GL_TRUE) {
    printf("Failed to open a GLFW window.\n");

    glfwTerminate();
    return 0;
  }

  glfwSetWindowTitle("CEN64");
  glfwSetWindowCloseCallback(CloseRequested);
  glfwPollEvents();

  if ((device = CreateDevice(argv[1])) == NULL) {
    printf("Failed to create a device.\n");

    glfwCloseWindow();
    return 1;
  }

  if (LoadCartridge(device, argv[2])) {
    printf("Failed to load the ROM.\n");

    DestroyDevice(device);
    return 2;
  }

  /* Parse the argument list now that */
  /* the console is ready for us. */
  ParseArgs(argc - 3, argv + 3, device);

  debug("== Booting the Console ==");

  if (setjmp(env) == 0) {
    while (1)
      CycleDevice(device);
  }

  debug("== Destroying the Console ==");

  DestroyDevice(device);
  glfwCloseWindow();
  glfwTerminate();
  return 0;
}

