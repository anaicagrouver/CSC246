public class Interleaving {
  /** Thread to print ab. */

static class MyRunnable implements Runnable {
  public void run() {
    System.out.print("ab");
  }
}

  /** Thread to print cd. */

static class MyRunnable1 implements Runnable {
  public void run() {
    System.out.print("cd");
  }
}


  /** Thread to print ef. */

static class MyRunnable2 implements Runnable {
  public void run() {
    System.out.print("ef");
  }
}

  public static void main( String[] args ) {
    // The three threads we make.
    Thread tab, tcd, tef;
  
    // A bunch of times.
    for ( int i = 0; i < 50000; i++ ) {
      // Make one of each type of threads.
      
      tab = new Thread( new MyRunnable());
      tcd = new Thread( new MyRunnable1());
      tef = new Thread( new MyRunnable2());
    
      // Start them all.

      tab.start();
      tcd.start();
      tef.start();

      // Join with our three threads.

    try {
      tab.join();
      tcd.join();
      tef.join();
    } catch ( InterruptedException e ) {
      System.out.println( "Can't join thread" );
    }

      // Print a newline to end the line they just printed.
      System.out.println();
    }
  }
}