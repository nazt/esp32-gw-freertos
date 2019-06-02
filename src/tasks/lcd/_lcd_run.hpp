void CMMC_LCD::paintRunPage() {
  // SERIAL0.println("DRAWING..");
  static int ct = 0;
  u8g2->drawXBM(7, 0, 30, 36, logo);

  u8g2->setFont(u8g2_font_p01type_tn);
  u8g2->setCursor(4, 6);
  // uint32_t lastSentInSeconds = (millis() - modem->lastSentOkMillis)/1000;
  u8g2->print(ct++);

  u8g2->setFont(u8g2_font_siji_t_6x10);
  u8g2->setCursor(logoMargin + 6, 16 + 1);
  u8g2->print("DustBoy 2.0");

  u8g2->setFont(u8g2_font_siji_t_6x10);
  u8g2->setCursor(logoMargin + 6, 27 + 1);
  u8g2->print("NB-IoT");

  u8g2->setFont(u8g2_font_micro_tr);
  u8g2->setCursor(logoMargin + 44, 27 + 1);
  // u8g2->print(modem->getStatus());
  // u8g2->setFont(u8g2_font_logisoso16_tf);
  u8g2->setFont(u8g2_font_siji_t_6x10);
  u8g2->setCursor(marginLeft, 50);
  u8g2->print("PM10");
  u8g2->setCursor(marginLeft, 62);
  u8g2->print("PM2.5");

  u8g2->setCursor(marginLeft + 35, 50);
  u8g2->print((pm10));
  u8g2->setCursor(marginLeft + 35, 62);
  // u8g2->print(String(pm2_5));
  u8g2->print((pm2_5));

  u8g2->setCursor(marginLeft + 75, 50);
  u8g2->print("ug/m3");
  u8g2->setCursor(marginLeft + 75, 62);
  u8g2->print("ug/m3");
  // u8g2->print("%");
}
