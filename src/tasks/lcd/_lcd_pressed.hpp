void CMMC_LCD::paintPressedButton() {
    int lineSpacing = 2;
    int factor = micros() % 6;
    factor = +0;
    u8g2->setCursor(5, 35 + (4 * lineSpacing));
    u8g2->setFont(u8g2_font_siji_t_6x10);
    u8g2->print("LCD PRESSED.");
}
