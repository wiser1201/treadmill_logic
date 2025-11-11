#ifndef LCD_H
#define LCD_H

void lcd_init(void);
void lcd_setCalories(const int calories);
void lcd_setSpeed(const int kmh);
void lcd_setTime(const int sec);
void lcd_setDistance(const int km);

#endif // LCD_H