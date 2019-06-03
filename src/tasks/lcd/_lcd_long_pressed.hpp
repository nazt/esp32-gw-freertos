void CMMC_LCD::paintLongPressedButton() {
      int lineSpacing = 2;
      u8g2->setFont(u8g2_font_siji_t_6x10);
      u8g2->setCursor(5, 35 + (4 * lineSpacing));
      u8g2->setFont(u8g2_font_siji_t_6x10);
      u8g2->print("LCD LONG PRESSED... Restarting..");
}
