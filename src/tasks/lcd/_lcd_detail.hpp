extern char magel_token[40];

void CMMC_LCD::paintDetailPage() {
  static int ct = 0;
  int logoMargin = 40;
  int lineSpacing = 2;
  char numBuffer[20];

  // uint32_t lastSentInSeconds = (millis() - modem->lastSentOkMillis)/1000;
  u8g2->setFont(u8g2_font_micro_tr);
  // u8g2->setCursor(logoMargin+10, 16);
  u8g2->setCursor(5, 15);
  u8g2->print("Local Packet Recv");
  u8g2->setCursor(80 + 5, 15);
  u8g2->print(formatedNumber(numBuffer, packetRecv));


  u8g2->setCursor(5, 20 + (1 * lineSpacing));
  u8g2->print("Keep Alive Packet");
  u8g2->setCursor(80 + 5, 20 + (1 * lineSpacing));
  u8g2->print(formatedNumber(numBuffer, keepAliveSent));


  u8g2->setCursor(5, 25 + (2 * lineSpacing));
  u8g2->print("NB-IoT Packet Sent");
  u8g2->setCursor(80 + 5, 25 + (2 * lineSpacing));
  u8g2->print(formatedNumber(numBuffer, nbSentOk));

  u8g2->setCursor(5, 30 + (3 * lineSpacing));
  u8g2->print("Reboot");
  u8g2->setCursor(80 + 5, 30 + (3 * lineSpacing));
  u8g2->print(formatedNumber(numBuffer, rebootCount));

  u8g2->setCursor(5, 35 + (4 * lineSpacing));
  u8g2->print("uptime ");

  char uptimeBuffer[40];
  sprintf(uptimeBuffer, "%dd,%02dh,%02dm,%02ds", day() - 1, hour(), minute(), second());
  u8g2->setCursor(40 + 5, 35 + (4 * lineSpacing));
  u8g2->print(uptimeBuffer);


  char statusBuffer[60];
  lineSpacing = 2;
  sprintf(statusBuffer, "Status: (Queue=%d)", pArrIdx);
  u8g2->setCursor(5, 40 + (5 * lineSpacing));
  u8g2->print(statusBuffer);

  u8g2->setCursor(5, 45 + (6 * lineSpacing));
  u8g2->print(magel_token);

  // u8g2->setFont(u8g2_font_p01type_tn);
  // u8g2->setFont(u8g2_font_micro_tr);
  // u8g2->setFont(u8g2_font_unifont_t_symbols);
  // // https://github.com/olikraus/u8g2/wiki/u8g2reference
  // u8g2->drawGlyph(0 + 4, 1f0, 9680 + count % 7);
  // u8g2->setCursor(0, 7);

  u8g2->setFont(u8g2_font_p01type_tn);
  u8g2->setCursor(126, 63);
  u8g2->print( (ct++)%2 ? ".": " ");
}
