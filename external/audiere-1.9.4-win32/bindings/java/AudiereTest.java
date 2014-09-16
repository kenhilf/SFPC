package org.aegisknight.audiere;

public class AudiereTest {
  public static void main(String[] args) {
    try {
      AudioDevice device = new AudioDevice();
      String name = "M:/mpeg/Other/The Cure/Wish/01 - Open.ogg";
      OutputStream stream = device.openSound(name, true);
      stream.play();
      Thread.currentThread().sleep(10000);
    }
    catch (Exception e) {
      System.out.println(e);
    }
  }
}
