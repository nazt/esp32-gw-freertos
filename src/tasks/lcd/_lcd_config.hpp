void CMMC_LCD::paintConfigPage() {
      char statusBuffer[60];
      int lineSpacing = 2;
      sprintf(statusBuffer, "AP=%s", this->_ap_name);
      u8g2->setFont(u8g2_font_siji_t_6x10);
      u8g2->setCursor(5, 64/2 + (4 * lineSpacing)/2);
      u8g2->setFont(u8g2_font_siji_t_6x10);
      // u8g2->print("IN CONFIG MODE.");
      u8g2->print(statusBuffer);
}
