/*
*  C Implementation: SDL_extras
*
* Description: a few handy functions for using SDL graphics.
*
*
* Author: David Bruce,,, <davidstuartbruce@gmail.com> (C) 2007
*
* Copyright: GPL v3 or later
*
*/
#include <math.h>

#include "SDL_extras.h"
#include "tuxhistory.h"
#include "loaders.h"
#include "pixels.h"
#include "options.h"



/* DrawButton() creates a translucent button with rounded ends
   and draws it on the screen.
   All colors and alpha values are supported.*/
void DrawButton(SDL_Rect* target_rect,
                int radius,
                Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  SDL_Surface* tmp_surf = CreateButton(target_rect->w, target_rect->h,
                                       radius, r, g, b, a);
  SDL_BlitSurface(tmp_surf, NULL, screen, target_rect);
  SDL_FreeSurface(tmp_surf);
}

/* CreateButton() creates a translucent button with rounded ends
   All colors and alpha values are supported.*/
SDL_Surface* CreateButton(int w, int h, int radius,
                          Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
  /* NOTE - we use a 32-bit temp surface even if we have a 16-bit */
  /* screen - it gets converted during blitting.                  */
  SDL_Surface* tmp_surf = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,
                                          w,
                                          h,
                                          32,
                                          rmask, gmask, bmask, amask);

  Uint32 color = SDL_MapRGBA(tmp_surf->format, r, g, b, a);
  SDL_FillRect(tmp_surf, NULL, color);
  RoundCorners(tmp_surf, radius);
  return tmp_surf;
}


void RoundCorners(SDL_Surface* s, Uint16 radius)
{
  int y = 0;
  int x_dist, y_dist;
  Uint32* p = NULL;
  Uint32 alpha_mask;
  int bytes_per_pix;

  if (!s)
    return;
  if (SDL_LockSurface(s) == -1)
    return;

  bytes_per_pix = s->format->BytesPerPixel;
  if (bytes_per_pix != 4)
    return;

  /* radius cannot be more than half of width or height: */
  if (radius > (s->w)/2)
    radius = (s->w)/2;
  if (radius > (s->h)/2)
    radius = (s->h)/2;


  alpha_mask = s->format->Amask;

  /* Now round off corners: */
  /* upper left:            */
  for (y = 0; y < radius; y++)
  {
    p = (Uint32*)(s->pixels + (y * s->pitch));
    x_dist = radius;
    y_dist = radius - y;

    while (((x_dist * x_dist) + (y_dist * y_dist)) > (radius * radius))
    {
      /* (make pixel (x,y) transparent) */
      *p = *p & ~alpha_mask;
      p++;
      x_dist--;
    }
  }

  /* upper right:            */
  for (y = 0; y < radius; y++)
  {
    /* start at end of top row: */
    p = (Uint32*)(s->pixels + ((y + 1) * s->pitch) - bytes_per_pix);

    x_dist = radius;
    y_dist = radius - y;

    while (((x_dist * x_dist) + (y_dist * y_dist)) > (radius * radius))
    {
      /* (make pixel (x,y) transparent) */
      *p = *p & ~alpha_mask;
      p--;
      x_dist--;
    }
  }

  /* bottom left:            */
  for (y = (s->h - 1); y > (s->h - radius); y--)
  {
    /* start at beginning of bottom row */
    p = (Uint32*)(s->pixels + (y * s->pitch));
    x_dist = radius;
    y_dist = y - (s->h - radius);

    while (((x_dist * x_dist) + (y_dist * y_dist)) > (radius * radius))
    {
      /* (make pixel (x,y) transparent) */
      *p = *p & ~alpha_mask;
      p++;
      x_dist--;
    }
  }

  /* bottom right:            */
  for (y = (s->h - 1); y > (s->h - radius); y--)
  {
    /* start at end of bottom row */
    p = (Uint32*)(s->pixels + ((y + 1) * s->pitch) - bytes_per_pix);
    x_dist = radius;
    y_dist = y - (s->h - radius);

    while (((x_dist * x_dist) + (y_dist * y_dist)) > (radius * radius))
    {
      /* (make pixel (x,y) transparent) */
      *p = *p & ~alpha_mask;
      p--;
      x_dist--;
    }
  }
  SDL_UnlockSurface(s);
}


/**********************
 Flip:
   input: a SDL_Surface, x, y
   output: a copy of the SDL_Surface flipped via rules:

     if x is a nonzero value, then flip horizontally
     if y is a nonzero value, then flip vertically

     note: you can have it flip both
**********************/
SDL_Surface* Flip( SDL_Surface *in, int x, int y ) {
        SDL_Surface *out, *tmp;
        SDL_Rect from_rect, to_rect;
        Uint32        flags;
        Uint32  colorkey=0;

        /* --- grab the settings for the incoming pixmap --- */

        SDL_LockSurface(in);
        flags = in->flags;

        /* --- change in's flags so ignore colorkey & alpha --- */

        if (flags & SDL_SRCCOLORKEY) {
                in->flags &= ~SDL_SRCCOLORKEY;
                colorkey = in->format->colorkey;
        }
        if (flags & SDL_SRCALPHA) {
                in->flags &= ~SDL_SRCALPHA;
        }

        SDL_UnlockSurface(in);

        /* --- create our new surface --- */

        out = SDL_CreateRGBSurface(
                SDL_SWSURFACE,
                in->w, in->h, 32, rmask, gmask, bmask, amask);

        /* --- flip horizontally if requested --- */

        if (x) {
                from_rect.h = to_rect.h = in->h;
                from_rect.w = to_rect.w = 1;
                from_rect.y = to_rect.y = 0;
                from_rect.x = 0;
                to_rect.x = in->w - 1;

                do {
                        SDL_BlitSurface(in, &from_rect, out, &to_rect);
                        from_rect.x++;
                        to_rect.x--;
                } while (to_rect.x >= 0);
        }

        /* --- flip vertically if requested --- */

        if (y) {
                from_rect.h = to_rect.h = 1;
                from_rect.w = to_rect.w = in->w;
                from_rect.x = to_rect.x = 0;
                from_rect.y = 0;
                to_rect.y = in->h - 1;

                do {
                        SDL_BlitSurface(in, &from_rect, out, &to_rect);
                        from_rect.y++;
                        to_rect.y--;
                } while (to_rect.y >= 0);
        }

        /* --- restore colorkey & alpha on in and setup out the same --- */

        SDL_LockSurface(in);

        if (flags & SDL_SRCCOLORKEY) {
                in->flags |= SDL_SRCCOLORKEY;
                in->format->colorkey = colorkey;
                tmp = SDL_DisplayFormat(out);
                SDL_FreeSurface(out);
                out = tmp;
                out->flags |= SDL_SRCCOLORKEY;
                out->format->colorkey = colorkey;
        } else if (flags & SDL_SRCALPHA) {
                in->flags |= SDL_SRCALPHA;
                tmp = SDL_DisplayFormatAlpha(out);
                SDL_FreeSurface(out);
                out = tmp;
        } else {
                tmp = SDL_DisplayFormat(out);
                SDL_FreeSurface(out);
                out = tmp;
        }

        SDL_UnlockSurface(in);

        return out;
}

/* Blend two surfaces together. The third argument is between 0.0 and
   1.0, and represents the weight assigned to the first surface.  If
   the pointer to the second surface is NULL, this performs fading.

   Currently this works only with RGBA images, but this is largely to
   make the (fast) pointer arithmetic work out; it could be easily
   generalized to other image types. */
SDL_Surface* Blend(SDL_Surface *S1, SDL_Surface *S2, float gamma)
{
  SDL_PixelFormat *fmt1, *fmt2;
  Uint8 r1, r2, g1, g2, b1, b2, a1, a2;
  SDL_Surface *tmpS, *ret;
  Uint32 *cpix1, *epix1, *cpix2, *epix2;
  float gamflip;

  if (!S1)
    return NULL;

  fmt1 = fmt2 = NULL;
  tmpS = ret = NULL;

  gamflip = 1.0 - gamma;
  if (gamma < 0 || gamflip < 0)
  {
    perror("gamma must be between 0 and 1");
    exit(0);
  }

  fmt1 = S1->format;

  if (fmt1 && fmt1->BitsPerPixel != 32)
  {
    perror("This works only with RGBA images");
    return S1;
  }
  if (S2 != NULL)
  {
    fmt2 = S2->format;
    if (fmt2->BitsPerPixel != 32)
    {
      perror("This works only with RGBA images");
      return S1;
    }
    // Check that both images have the same width dimension
    if (S1->w != S2->w)
    {
      printf("S1->w %d, S2->w %d;  S1->h %d, S2->h %d\n",
             S1->w, S2->w, S1->h, S2->h);
      printf("Both images must have the same width dimensions\n");
      return S1;
    }
  }

  tmpS = SDL_ConvertSurface(S1, fmt1, SDL_SWSURFACE);
  if (tmpS == NULL)
  {
    perror("SDL_ConvertSurface() failed");
    return S1; 
  }
  if (-1 == SDL_LockSurface(tmpS))
  {
    perror("SDL_LockSurface() failed");
    return S1; 
  }

  // We're going to go through the pixels in reverse order, to start
  // from the bottom of each image. That way, we can blend things that
  // are not of the same height and have them align at the bottom.
  // So the "ending pixel" (epix) will be before the first pixel, and
  // the current pixel (cpix) will be the last pixel.
  epix1 = (Uint32*) tmpS->pixels - 1;
  cpix1 = epix1 + tmpS->w * tmpS->h;
  if (S2 != NULL
      && (SDL_LockSurface(S2) != -1))
  {
    epix2 = (Uint32*) S2->pixels - 1;
    cpix2 = epix2 + S2->w * S2->h;
  }
  else
  {
    epix2 = epix1;
    cpix2 = cpix1;
  }

  for (; cpix1 > epix1; cpix1--, cpix2--)
  {
    SDL_GetRGBA(*cpix1, fmt1, &r1, &g1, &b1, &a1);
    a1 = gamma * a1;
    if (S2 != NULL && cpix2 > epix2)
    {
      SDL_GetRGBA(*cpix2, fmt2, &r2, &g2, &b2, &a2);
      r1 = gamma * r1 + gamflip * r2;
      g1 = gamma * g1 + gamflip * g2;
      b1 = gamma * b1 + gamflip * b2;
      a1 += gamflip * a2;
    }
    *cpix1 = SDL_MapRGBA(fmt1,r1,g1,b1,a1);
  }

  SDL_UnlockSurface(tmpS);

  if (S2 != NULL)
    SDL_UnlockSurface(S2);

  ret = SDL_DisplayFormatAlpha(tmpS);
  SDL_FreeSurface(tmpS);

  return ret;
}


/* free every surface in the array together with the array itself */
void FreeSurfaceArray(SDL_Surface** surfs, int length)
{
  int i;

  if(surfs == NULL)
    return;

  for(i = 0; i < length; i++)
    if(surfs[i] != NULL)
      SDL_FreeSurface(surfs[i]);
  free(surfs);
}

int inRect( SDL_Rect r, int x, int y) {
        if ((x < r.x) || (y < r.y) || (x > r.x + r.w) || (y > r.y + r.h))
                return 0;
        return 1;
}

void UpdateRect(SDL_Surface* surf, SDL_Rect* rect)
{
  SDL_UpdateRect(surf, rect->x, rect->y, rect->w, rect->h);
}

void SetRect(SDL_Rect* rect, const float* pos)
{
  rect->x = pos[0] * screen->w;
  rect->y = pos[1] * screen->h;
  rect->w = pos[2] * screen->w;
  rect->h = pos[3] * screen->h;
}

/* Darkens the screen by a factor of 2^bits */
void DarkenScreen(Uint8 bits)
{
#if PIXEL_BITS == 32
  Uint32* p;
#elif PIXEL_BITS == 16
  Uint16* p;
#else
  return;
#endif
  Uint32 rm = screen->format->Rmask;
  Uint32 gm = screen->format->Gmask;
  Uint32 bm = screen->format->Bmask;


  int x, y;

  /* (realistically, 1 and 2 are the only useful values) */
  if (bits > 8)
    return;

  p = screen->pixels;

  for (y = 0; y < screen->h; y++)
  {
    for (x = 0; x < screen->w; x++)
    {
      *p = (((*p&rm)>>bits)&rm)
         | (((*p&gm)>>bits)&gm)
         | (((*p&bm)>>bits)&bm);
      p++;
    }
  }
}

/* change window size (works only in windowed mode) */
void ChangeWindowSize(int new_res_x, int new_res_y)
{
  SDL_Surface* oldscreen = screen;

  if(!(screen->flags & SDL_FULLSCREEN))
  {
    screen = SDL_SetVideoMode(new_res_x,
                              new_res_y,
                              PIXEL_BITS,
                              SDL_SWSURFACE|SDL_HWPALETTE);

    if(screen == NULL)
    {
      fprintf(stderr,
              "\nError: I could not change screen mode into %d x %d.\n",
              new_res_x, new_res_y);
      screen = oldscreen;
    }
    else
    {
      DEBUGMSG(debug_sdl, "ChangeWindowSize(): Changed window size to %d x %d\n", screen->w, screen->h);
      oldscreen = NULL;
      win_res_x = screen->w;
      win_res_y = screen->h;
      SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
  }
  else
    DEBUGMSG(debug_sdl, "ChangeWindowSize() can be run only in windowed mode !");
}

/* switch between fullscreen and windowed mode */
void SwitchScreenMode(void)
{
  int window = (screen->flags & SDL_FULLSCREEN);
  SDL_Surface* oldscreen = screen;

  screen = SDL_SetVideoMode(window ? win_res_x : fs_res_x,
                            window ? win_res_y : fs_res_y,
                            PIXEL_BITS,
                            screen->flags ^ SDL_FULLSCREEN);

  if (screen == NULL)
  {
    fprintf(stderr,
            "\nError: I could not switch to %s mode.\n"
            "The Simple DirectMedia error that occured was:\n"
            "%s\n\n",
            window ? "windowed" : "fullscreen",
            SDL_GetError());
    screen = oldscreen;
  }
  else
  {
    //success, no need to free the old video surface
    DEBUGMSG(debug_sdl, "Switched screen mode to %s\n", window ? "windowed" : "fullscreen");
    oldscreen = NULL;
    SDL_UpdateRect(screen, 0, 0, 0, 0);
  }
}

/*
Block application until SDL receives an appropriate event. Events can be
a single or OR'd combination of event masks. 
e.g. e = WaitForEvent(SDL_KEYDOWNMASK | SDL_QUITMASK)
*/
SDL_EventType WaitForEvent(SDL_EventMask events)
{
  SDL_Event evt;
  while (1)
  {
    while (SDL_PollEvent(&evt) )
    {
      if (SDL_EVENTMASK(evt.type) & events)
        return evt.type;
      else 
        SDL_Delay(50);
    }
  }
}
/* Swiped shamelessly from TuxPaint
   Based on code from: http://www.codeproject.com/cs/media/imageprocessing4.asp
   copyright 2002 Christian Graus */

SDL_Surface* zoom(SDL_Surface* src, int new_w, int new_h)
{
  SDL_Surface* s;

  /* These function pointers will point to the appropriate */
  /* putpixel() and getpixel() variants to be used in the  */
  /* current colorspace:                                   */
  void (*putpixel) (SDL_Surface*, int, int, Uint32);
  Uint32(*getpixel) (SDL_Surface*, int, int);

  float xscale, yscale;
  int x, y;
  int floor_x, ceil_x,
        floor_y, ceil_y;
  float fraction_x, fraction_y,
        one_minus_x, one_minus_y;
  float n1, n2;
  Uint8 r1, g1, b1, a1;
  Uint8 r2, g2, b2, a2;
  Uint8 r3, g3, b3, a3;
  Uint8 r4, g4, b4, a4;
  Uint8 r, g, b, a;

  DEBUGMSG(debug_sdl, "Entering zoom():\n");

  /* Create surface for zoom: */

  s = SDL_CreateRGBSurface(src->flags,        /* SDL_SWSURFACE, */
                           new_w, new_h, src->format->BitsPerPixel,
                           src->format->Rmask,
                           src->format->Gmask,
                           src->format->Bmask,
                           src->format->Amask);

  if (s == NULL)
  {
    fprintf(stderr, "\nError: Can't build zoom surface\n"
            "The Simple DirectMedia Layer error that occurred was:\n"
            "%s\n\n", SDL_GetError());
    return NULL;
//    cleanup();
//    exit(1);
  }

  DEBUGMSG(debug_sdl, "zoom(): orig surface %dx%d, %d bytes per pixel\n",
            src->w, src->h, src->format->BytesPerPixel);
  DEBUGMSG(debug_sdl, "zoom(): new surface %dx%d, %d bytes per pixel\n",
            s->w, s->h, s->format->BytesPerPixel);

  /* Now assign function pointers to correct functions based */
  /* on data format of original and zoomed surfaces:         */
  getpixel = getpixels[src->format->BytesPerPixel];
  putpixel = putpixels[s->format->BytesPerPixel];

  SDL_LockSurface(src);
  SDL_LockSurface(s);

  xscale = (float) src->w / (float) new_w;
  yscale = (float) src->h / (float) new_h;

  for (x = 0; x < new_w; x++)
  {
    for (y = 0; y < new_h; y++)
    {
      /* Here we calculate the new RGBA values for each pixel */
      /* using a "weighted average" of the four pixels in the */
      /* corresponding location in the orginal surface:       */

      /* figure out which original pixels to use in the calc: */
      floor_x = floor((float) x * xscale);
      ceil_x = floor_x + 1;
      if (ceil_x >= src->w)
        ceil_x = floor_x;

      floor_y = floor((float) y * yscale);
      ceil_y = floor_y + 1;
      if (ceil_y >= src->h)
        ceil_y = floor_y;

      fraction_x = x * xscale - floor_x;
      fraction_y = y * yscale - floor_y;

      one_minus_x = 1.0 - fraction_x;
      one_minus_y = 1.0 - fraction_y;

      /* Grab their values:  */
      SDL_GetRGBA(getpixel(src, floor_x, floor_y), src->format,
                  &r1, &g1, &b1, &a1);
      SDL_GetRGBA(getpixel(src, ceil_x,  floor_y), src->format,
                  &r2, &g2, &b2, &a2);
      SDL_GetRGBA(getpixel(src, floor_x, ceil_y),  src->format,
                  &r3, &g3, &b3, &a3);
      SDL_GetRGBA(getpixel(src, ceil_x,  ceil_y),  src->format,
                  &r4, &g4, &b4, &a4);

      /* Create the weighted averages: */
      n1 = (one_minus_x * r1 + fraction_x * r2);
      n2 = (one_minus_x * r3 + fraction_x * r4);
      r = (one_minus_y * n1 + fraction_y * n2);

      n1 = (one_minus_x * g1 + fraction_x * g2);
      n2 = (one_minus_x * g3 + fraction_x * g4);
      g = (one_minus_y * n1 + fraction_y * n2);

      n1 = (one_minus_x * b1 + fraction_x * b2);
      n2 = (one_minus_x * b3 + fraction_x * b4);
      b = (one_minus_y * n1 + fraction_y * n2);

      n1 = (one_minus_x * a1 + fraction_x * a2);
      n2 = (one_minus_x * a3 + fraction_x * a4);
      a = (one_minus_y * n1 + fraction_y * n2);

      /* and put them into our new surface: */
      putpixel(s, x, y, SDL_MapRGBA(s->format, r, g, b, a));

    }
  }

  SDL_UnlockSurface(s);
  SDL_UnlockSurface(src);

  DEBUGMSG(debug_sdl, "Leaving zoom():\n");

  return s;
}

/************************************************************************/
/*                                                                      */
/*        Begin text drawing functions                                  */
/*                                                                      */
/* These functions support text drawing using either SDL_Pango          */
/* or SDL_ttf. SDL_Pango is preferable but is not available on all      */
/* platforms. Code outside of this file does not have to worry about    */
/* which library is used to do the actual rendering.                    */
/************************************************************************/

#define MAX_FONT_SIZE 40

//NOTE to test program with SDL_ttf, do "./configure --without-sdlpango"


/*-- file-scope variables and local file prototypes for SDL_Pango-based code: */
#ifdef HAVE_LIBSDL_PANGO
#include "SDL_Pango.h"
SDLPango_Context* context = NULL;
static SDLPango_Matrix* SDL_Colour_to_SDLPango_Matrix(const SDL_Color* cl);
static int Set_SDL_Pango_Font_Size(int size);

/*-- file-scope variables and local file prototypes for SDL_ttf-based code: */
#else
#include "SDL_ttf.h"
/* We cache fonts here once loaded to improve performance: */
TTF_Font* font_list[MAX_FONT_SIZE + 1] = {NULL};
static void free_font_list(void);
static TTF_Font* get_font(int size);
static TTF_Font* load_font(const char* font_name, int font_size);
#endif


/* "Public" functions called from other files that use either */
/*SDL_Pango or SDL_ttf:                                       */


/* For setup, we either initialize SDL_Pango and set its context, */
/* or we initialize SDL_ttf:                                      */
int Setup_SDL_Text(void)
{
#ifdef HAVE_LIBSDL_PANGO

  DEBUGMSG(debug_sdl, "Setup_SDL_Text() - using SDL_Pango\n");

  SDLPango_Init();
  if (!Set_SDL_Pango_Font_Size(DEFAULT_MENU_FONT_SIZE))
  {
    fprintf(stderr, "\nError: I could not set SDL_Pango context\n");
    return 0;
  }
  return 1;

#else
/* using SDL_ttf: */
  DEBUGMSG(debug_sdl, "Setup_SDL_Text() - using SDL_ttf\n");

  if (TTF_Init() < 0)
  {
    fprintf(stderr, "\nError: I could not initialize SDL_ttf\n");
    return 0;
  }
  return 1;
#endif
}



void Cleanup_SDL_Text(void)
{
#ifdef HAVE_LIBSDL_PANGO
  if(context != NULL)
    SDLPango_FreeContext(context);
  context = NULL;
#else
  free_font_list();
  TTF_Quit();
#endif
}


/* BlackOutline() creates a surface containing text of the designated */
/* foreground color, surrounded by a black shadow, on a transparent    */
/* background.  The appearance can be tuned by adjusting the number of */
/* background copies and the offset where the foreground text is       */
/* finally written (see below).                                        */
//SDL_Surface* BlackOutline(const char *t, TTF_Font *font, SDL_Color *c)
SDL_Surface* BlackOutline(const char* t, int size, SDL_Color* c)
{
  SDL_Surface* out = NULL;
  SDL_Surface* black_letters = NULL;
  SDL_Surface* white_letters = NULL;
  SDL_Surface* bg = NULL;
  SDL_Rect dstrect;
  Uint32 color_key;

/* Make sure everything is sane before we proceed: */
#ifdef HAVE_LIBSDL_PANGO
  if (!context)
  {
    fprintf(stderr, "BlackOutline(): invalid SDL_Pango context - returning.\n");
    return NULL;
  }
#else
  TTF_Font* font = get_font(size);
  if (!font)
  {
    fprintf(stderr, "BlackOutline(): could not load needed font - returning.\n");
    return NULL;
  }
#endif

  if (!t || !c)
  {
    fprintf(stderr, "BlackOutline(): invalid ptr parameter, returning.\n");
    return NULL;
  }

  if (t[0] == '\0')
  {
    fprintf(stderr, "BlackOutline(): empty string, returning\n");
    return NULL;
  }

  DEBUGMSG(debug_sdl, "Entering BlackOutline():\n");
  DEBUGMSG(debug_sdl, "BlackOutline of \"%s\"\n", t );

#ifdef HAVE_LIBSDL_PANGO
  Set_SDL_Pango_Font_Size(size);
  SDLPango_SetDefaultColor(context, MATRIX_TRANSPARENT_BACK_BLACK_LETTER);
  SDLPango_SetText(context, t, -1);
  black_letters = SDLPango_CreateSurfaceDraw(context);
#else
  black_letters = TTF_RenderUTF8_Blended(font, t, black);
#endif

  if (!black_letters)
  {
    fprintf (stderr, "Warning - BlackOutline() could not create image for %s\n", t);
    return NULL;
  }

  bg = SDL_CreateRGBSurface(SDL_SWSURFACE,
                            (black_letters->w) + 5,
                            (black_letters->h) + 5,
                             32,
                             rmask, gmask, bmask, amask);
  /* Use color key for eventual transparency: */
  color_key = SDL_MapRGB(bg->format, 01, 01, 01);
  SDL_FillRect(bg, NULL, color_key);

  /* Now draw black outline/shadow 2 pixels on each side: */
  dstrect.w = black_letters->w;
  dstrect.h = black_letters->h;

  /* NOTE: can make the "shadow" more or less pronounced by */
  /* changing the parameters of these loops.                */
  for (dstrect.x = 1; dstrect.x < 4; dstrect.x++)
    for (dstrect.y = 1; dstrect.y < 3; dstrect.y++)
      SDL_BlitSurface(black_letters , NULL, bg, &dstrect );

  SDL_FreeSurface(black_letters);

  /* --- Put the color version of the text on top! --- */
#ifdef HAVE_LIBSDL_PANGO
  /* convert color arg: */
  SDLPango_Matrix* color_matrix = SDL_Colour_to_SDLPango_Matrix(c);

  if (color_matrix)
  {
    SDLPango_SetDefaultColor(context, color_matrix);
    free(color_matrix);
  }
  else  /* fall back to just using white if conversion fails: */
    SDLPango_SetDefaultColor(context, MATRIX_TRANSPARENT_BACK_WHITE_LETTER);

  white_letters = SDLPango_CreateSurfaceDraw(context);

#else
  white_letters = TTF_RenderUTF8_Blended(font, t, *c);
#endif

  if (!white_letters)
  {
    fprintf (stderr, "Warning - BlackOutline() could not create image for %s\n", t);
    return NULL;
  }

  dstrect.x = 1;
  dstrect.y = 1;
  SDL_BlitSurface(white_letters, NULL, bg, &dstrect);
  SDL_FreeSurface(white_letters);

  /* --- Convert to the screen format for quicker blits --- */
  SDL_SetColorKey(bg, SDL_SRCCOLORKEY|SDL_RLEACCEL, color_key);
  out = SDL_DisplayFormatAlpha(bg);
  SDL_FreeSurface(bg);

  DEBUGMSG(debug_sdl, "\nLeaving BlackOutline(): \n");

  return out;
}


/* This (fast) function just returns a non-outlined surf */
/* using either SDL_Pango or SDL_ttf                     */
SDL_Surface* SimpleText(const char *t, int size, SDL_Color* col)
{
  SDL_Surface* surf = NULL;

  if (!t||!col)
    return NULL;

#ifdef HAVE_LIBSDL_PANGO
  if (!context)
  {
    fprintf(stderr, "SimpleText() - context not valid!\n");
    return NULL;
  }
  else
  {
    SDLPango_Matrix colormatrix =
    {{
      {col->r,  col->r,  0,  0},
      {col->g,  col->g,  0,  0},
      {col->b,  col->b,  0,  0},
      {0,      255,      0,  0}
    }};
    Set_SDL_Pango_Font_Size(size);
    SDLPango_SetDefaultColor(context, &colormatrix );
    SDLPango_SetText(context, t, -1);
    surf = SDLPango_CreateSurfaceDraw(context);
  }

#else
  {
    TTF_Font* font = get_font(size);
    if (!font)
      return NULL;
    surf = TTF_RenderUTF8_Blended(font, t, *col);
  }
#endif

  return surf;
}

/* This (fast) function just returns a non-outlined surf */
/* using SDL_Pango if available, SDL_ttf as fallback     */
SDL_Surface* SimpleTextWithOffset(const char *t, int size, SDL_Color* col, int *glyph_offset)
{
  SDL_Surface* surf = NULL;

  if (!t||!col)
    return NULL;

#ifdef HAVE_LIBSDL_PANGO
  if (!context)
  {
    fprintf(stderr, "SimpleText() - context not valid!\n");
    return NULL;
  }
  else
  {
    SDLPango_Matrix colormatrix =
    {{
      {col->r,  col->r,  0,  0},
      {col->g,  col->g,  0,  0},
      {col->b,  col->b,  0,  0},
      {0,      255,      0,  0}
    }};
    Set_SDL_Pango_Font_Size(size);
    SDLPango_SetDefaultColor(context, &colormatrix );
    SDLPango_SetText(context, t, -1);
    surf = SDLPango_CreateSurfaceDraw(context);
    *glyph_offset = 0; // fixme?
  }

#else
  {
    TTF_Font* font = get_font(size);
    if (!font)
      return NULL;
    surf = TTF_RenderUTF8_Blended(font, t, *col);
    {
      int h;
      int hmax = 0;
      int len = strlen(t);
      int i;
      for (i = 0; i < len; i++)
      {
        TTF_GlyphMetrics(font, t[i], NULL, NULL, NULL, &h, NULL);
        if (h > hmax)
	  hmax = h;
      }
      *glyph_offset = hmax - TTF_FontAscent(font);
    }
  }
#endif

  return surf;
}



/*-----------------------------------------------------------*/
/* Local functions, callable only within SDL_extras, divided */
/* according with which text lib we are using:               */
/*-----------------------------------------------------------*/



#ifdef HAVE_LIBSDL_PANGO
/* Local functions when using SDL_Pango:   */


/* NOTE the scaling by 3/4 a few lines down represents a conversion from      */
/* the usual text dpi of 72 to the typical screen dpi of 96. It gives         */
/* font sizes fairly similar to a SDL_ttf font with the same numerical value. */
static int Set_SDL_Pango_Font_Size(int size)
{
  /* static so we can "remember" values from previous time through: */
  static int prev_pango_font_size;
  static char prev_font_name[FONT_NAME_LENGTH];
  /* Do nothing unless we need to change size or font: */
  if ((size == prev_pango_font_size)
      &&
      (0 == strncmp(prev_font_name, Opts_FontName(), sizeof(prev_font_name))))
    return 1;
  else
  {
    char buf[64];

    DEBUGMSG(debug_sdl, "Setting font size to %d\n", size);

    if(context != NULL)
      SDLPango_FreeContext(context);
    context = NULL;
    snprintf(buf, sizeof(buf), "%s %d", Opts_FontName(), (int)((size * 3)/4));
    context =  SDLPango_CreateContext_GivenFontDesc(buf);
  }

  if (!context)
    return 0;
  else
  {
    prev_pango_font_size = size;
    strncpy(prev_font_name, Opts_FontName(), sizeof(prev_font_name));
    return 1;
  }
}


SDLPango_Matrix* SDL_Colour_to_SDLPango_Matrix(const SDL_Color *cl)
{
  int k = 0;
  SDLPango_Matrix* colour = NULL;

  if (!cl)
  {
    fprintf(stderr, "Invalid SDL_Color* arg\n");
    return NULL;
  }

  colour = (SDLPango_Matrix*)malloc(sizeof(SDLPango_Matrix));

  for(k = 0; k < 4; k++)
  {
    (*colour).m[0][k] = (*cl).r;
    (*colour).m[1][k] = (*cl).g;
    (*colour).m[2][k] = (*cl).b;
  }
  (*colour).m[3][0] = 0;
  (*colour).m[3][1] = 255;
  (*colour).m[3][2] = 0;
  (*colour).m[3][3] = 0;

  return colour;
}

#else
/* Local functions when using SDL_ttf: */

static void free_font_list(void)
{
  int i;
  for(i = 0; i < MAX_FONT_SIZE; i++)
  {
    if(font_list[i])
    {
      TTF_CloseFont(font_list[i]);
      font_list[i] = NULL;
    }
  }
}

/* FIXME - could combine this with load_font() below:         */
/* Loads and caches fonts in each size as they are requested: */
/* We use the font size as an array index, keeping each size  */
/* font in memory once loaded until cleanup.                  */
static TTF_Font* get_font(int size)
{
  static char prev_font_name[FONT_NAME_LENGTH];
  if (size < 0)
  {
    fprintf(stderr, "Error - requested font size %d is negative\n", size);
    return NULL;
  }

  if (size > MAX_FONT_SIZE)
  {
    fprintf(stderr, "Error - requested font size %d exceeds max = %d, resetting.\n",
            size, MAX_FONT_SIZE);
    size = MAX_FONT_SIZE;
  }

  /* If the font has changed, we need to wipe out the old ones: */
  if (0 != strncmp(prev_font_name, Opts_FontName(),sizeof(prev_font_name)))
  {
    free_font_list();
    strncpy(prev_font_name, Opts_FontName(), sizeof(prev_font_name));
  }

  if(font_list[size] == NULL)
    font_list[size] = load_font(DEFAULT_FONT_NAME, size);
  return font_list[size];
}


/* FIXME: I think we need to provide a single default font with the program data, */
/* then more flexible code to try to locate or load system fonts. DSB             */
/* Returns ptr to loaded font if successful, NULL otherwise. */
static TTF_Font* load_font(const char* font_name, int font_size)
{
  TTF_Font* f;
  char fontfile[PATH_MAX];
  sprintf(fontfile, "%s/fonts/%s", DATA_PREFIX, font_name);

  f = TTF_OpenFont(fontfile, font_size);

  /* HACK - better font searching needed! */
  /* This should mean that font wasn't bundled into data path, which for  */
  /* now means we are using Debian, so grab from Debian installation loc: */
  if (!f)
  { 
    sprintf(fontfile, "/usr/share/fonts/truetype/ttf-sil-andika/AndikaDesRevG.ttf");
    f = TTF_OpenFont(fontfile, font_size);
  }


  if (f)
  {
    DEBUGMSG(debug_sdl, "LoadFont(): %s loaded successfully\n\n", fontfile);
    return f;
  }
  else
  {
   fprintf(stderr, "LoadFont(): %s NOT loaded successfully.\n", fontfile);
   return NULL;
  }
}

Uint32 get_pcolori(SDL_Surface *surface, int x, int y)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    return pixels[ ( y * surface->w ) + x ];
}

#endif

/* Draw a line: */
void draw_line(SDL_Surface* surface, SDL_Rect rect, int red, int grn, int blu)
{
  int dx, dy, tmp;
  float m, b;
  Uint32 pixel;
  SDL_Rect dest;

  pixel = SDL_MapRGB(screen->format, red, grn, blu);

  dx = rect.w - rect.x;
  dy = rect.h - rect.y;

  putpixel(screen, rect.x, rect.y, pixel);

  if (dx != 0)
  {
    m = ((float) dy) / ((float) dx);
    b = rect.y - m * rect.x;

    if (rect.w > rect.x)
      dx = 1;
    else
      dx = -1;

    while (rect.x != rect.w)
    {
      rect.x = rect.x + dx;
      rect.y = m * rect.x + b;

      putpixel(surface, rect.x, rect.y, pixel);
    }
  }
  else
  {
    if (rect.y > rect.h)
    {
      tmp = rect.y;
      rect.y = rect.h;
      rect.h = tmp;
    }

    dest.x = rect.x;
    dest.y = rect.y;
    dest.w = 3;
    dest.h = rect.h - rect.y;

    SDL_FillRect(surface, &dest, pixel);
  }
}


/* Draw a single pixel into the surface: */

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
#ifdef PUTPIXEL_RAW
  int bpp;
  Uint8* p;

  /* Determine bytes-per-pixel for the surface in question: */

  bpp = surface->format->BytesPerPixel;


  /* Set a pointer to the exact location in memory of the pixel
     in question: */

  p = (Uint8 *) (surface->pixels +       /* Start at beginning of RAM */
                 (y * surface->pitch) +  /* Go down Y lines */
                 (x * bpp));             /* Go in X pixels */


  /* Assuming the X/Y values are within the bounds of this surface... */

  if (x >= 0 && y >= 0 && x < surface->w && y < surface->h)
  {
      /* Set the (correctly-sized) piece of data in the surface's RAM
         to the pixel value sent in: */

    if (bpp == 1)
      *p = pixel;
    else if (bpp == 2)
      *(Uint16 *)p = pixel;
    else if (bpp == 3)
    {
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      }
      else
      {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
    }
    else if (bpp == 4)
    {
      *(Uint32 *)p = pixel;
    }
  }
#else
  SDL_Rect dest;

  dest.x = x;
  dest.y = y;
  dest.w = 3;
  dest.h = 4;

  SDL_FillRect(surface, &dest, pixel);
#endif
}

void draw_rect(SDL_Surface* surface, SDL_Rect rect)
{
    SDL_Rect tmp_rect;

    tmp_rect.x = rect.x;
    tmp_rect.y = rect.y;
    tmp_rect.w = rect.w;
    tmp_rect.h = rect.y;
    draw_line(surface, tmp_rect, 255, 255, 255);

    tmp_rect.x = rect.x;
    tmp_rect.y = rect.y;
    tmp_rect.w = rect.x;
    tmp_rect.h = rect.h;
    draw_line(surface, tmp_rect, 255, 255, 255);

    tmp_rect.x = rect.x;
    tmp_rect.y = rect.h;
    tmp_rect.w = rect.w;
    tmp_rect.h = rect.h;
    draw_line(surface, tmp_rect, 255, 255, 255);

    tmp_rect.x = rect.w;
    tmp_rect.y = rect.y;
    tmp_rect.w = rect.w;
    tmp_rect.h = rect.h;
    draw_line(surface, tmp_rect, 255, 255, 255);
}

void FillRect(SDL_Rect rect, int color) 
{
    SDL_FillRect(screen, &rect, color);
}

void th_ShowMessage(char* str, int size, int x, int y)
{
  SDL_Surface *s;
  SDL_Rect loc;

  s = NULL;

  if (str)
    s = BlackOutline(str, size, &white);

  /* Draw lines of text (do after drawing Tux so text is in front): */
  if (s)
  {
    loc.x = x; 
    loc.y = y;
    SDL_BlitSurface( s, NULL, screen, &loc);
  }

  /* and update: */
  //SDL_UpdateRect(screen, 0, 0, 0, 0);

  SDL_FreeSurface(s);
}


