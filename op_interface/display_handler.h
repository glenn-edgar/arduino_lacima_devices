#ifndef DISPLAY_HANDLER_H__
#define DISPLAY_HANDLER_H__


void initialize_display(void);
void lcd_flash(char *message,int on_time,int off_time);
void lcd_flash(const String &message,int on_time,int off_time);


void lcd_clear_screen(void);


void lcd_drawString(char *message,int x_pos,int y_pos);
void lcd_drawString(const String &message,int x_pos,int y_pos);


void lcd_setFreeFont(int size);











#endif
