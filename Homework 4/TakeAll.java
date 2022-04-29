import java.util.Random;

public class TakeAll {
  /** To tell all the chefs when they can quit running. */
  private static boolean running = true;

  /** Superclass for all chefs.  Contains methods to cook and rest and
      keeps a record of how many dishes were prepared. */
  private static class Chef extends Thread {
    /** Number of dishes prepared by this chef. */
    private int dishCount = 0;

    /** Source of randomness for this chef. */
    private Random rand = new Random();

    /** Called after the chef has locked all the required appliances and is
        ready to cook for about the given number of milliseconds. */
    protected void cook( int duration ) {
      System.out.printf( "%s is cooking\n", getClass().getSimpleName() );
      try {
        // Wait for a while (pretend to be cooking)
        Thread.sleep( rand.nextInt( duration / 2 ) + duration / 2 );
      } catch ( InterruptedException e ) {
      }
      dishCount++;
    }

    /** Called between dishes, to let the chef rest before cooking another dish. */
    protected void rest( int duration ) {
      System.out.printf( "%s is resting\n", getClass().getSimpleName() );
      try {
        // Wait for a while (pretend to be resting)
        Thread.sleep( rand.nextInt( duration / 2 ) + duration / 2 );
      } catch ( InterruptedException e ) {
      }
    }
  }

  // An object representing the lock on each appliance.
  // Locking the needed objects before cooking prevents two
  // chefs from trying to use the same appliance at the same time.
  // private static Object grill = new Object();
  // private static Object griddle = new Object();
  // private static Object blender = new Object();
  // private static Object oven = new Object();
  // private static Object microwave = new Object();
  // private static Object fryer = new Object();
  // private static Object mixer = new Object();
  // private static Object coffeeMaker = new Object();
  static boolean grill = false;
  static boolean griddle = false;
  static boolean blender = false;
  static boolean oven = false;
  static boolean microwave = false;
  static boolean fryer = false;
  static boolean mixer = false;
  static boolean coffeeMaker = false;
  
  //private static Object canCook = new Object();
  private static Object lock = new Object();


  /** Karon is a chef needing 75 milliseconds to prepare a dish. */
  private static class Karon extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized(lock){
           while(coffeeMaker || microwave || fryer){
           try{
             lock.wait(); 
           }
           catch(InterruptedException e){
             System.out.println("Unable to lock\n");
           }
          }
              coffeeMaker = true; 
              microwave = true; 
              fryer = true; 
        }
        cook(75);
        // synchronized(canCook){
        // cook(75);
        // }
        synchronized(lock){
            coffeeMaker = false; 
            microwave = false; 
            fryer = false; 
            lock.notifyAll();
          }
          rest( 25 );
      }
    }
  }

  /** Margret is a chef needing 15 milliseconds to prepare a dish. */
  private static class Margret extends Chef {
    public void run() {
      while ( running ) {
         synchronized(lock){
           while(coffeeMaker || grill || griddle){
            try{
              lock.wait(); 
            }
            catch(InterruptedException e){
              System.out.println("Unable to lock\n");
            }
          }
              coffeeMaker = true; 
              griddle = true; 
              grill = true; 
        }
        cook(15);
        // synchronized(canCook){
        //   cook(15);
        // }
          synchronized(lock){
              coffeeMaker = false; 
              griddle = false; 
              grill = false; 
              lock.notifyAll();
            }
          rest( 25 );
        }
      }
    }

  /** Brigette is a chef needing 105 milliseconds to prepare a dish. */
  private static class Brigette extends Chef {
    public void run() {
      while ( running ) {
         synchronized(lock){
           while(coffeeMaker || mixer){
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          coffeeMaker = true; 
          mixer = true; 
        }
        cook(105);
        // synchronized(canCook){
        //   cook(105);
        // }
          synchronized(lock){
              coffeeMaker = false; 
              mixer = false; 
              lock.notifyAll();
            }
          rest( 25 );
         }
      }
    }

  /** Elisabeth is a chef needing 30 milliseconds to prepare a dish. */
  private static class Elisabeth extends Chef {
    public void run() {
      while ( running ) {
      synchronized(lock){
           while(oven || griddle ||  blender){
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          oven = true; 
          griddle = true; 
          blender = true; 
        }
        cook(30);
        // synchronized(canCook){
        //   cook(30);
        // }
          synchronized(lock){
              oven = false; 
              griddle = false; 
              blender = false; 
              lock.notifyAll();
            }
        rest( 25 );
        }
      }
    }

  /** Teri is a chef needing 45 milliseconds to prepare a dish. */
  private static class Teri extends Chef {
    public void run() {
      while ( running ) {
        synchronized(lock){
           while(oven ||  blender){
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          oven = true; 
          blender = true; 
        }
        cook(45);
        // synchronized(canCook){
        //   cook(45);
        // }
          synchronized(lock){
              oven = false; 
              blender = false; 
              lock.notifyAll();
            }
        rest( 25 );
      }
    }
  }

  /** Belinda is a chef needing 90 milliseconds to prepare a dish. */
  private static class Belinda extends Chef {
    public void run() {
      while ( running ) {
          synchronized(lock){
           while(mixer || fryer){
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          fryer = true; 
          mixer = true; 
        }
        cook(90);
        // synchronized(canCook){
        //   cook(90);
        // }
          synchronized(lock){
              fryer = false; 
              mixer = false; 
              lock.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Eliza is a chef needing 15 milliseconds to prepare a dish. */
  private static class Eliza extends Chef {
    public void run() {
      while ( running ) {
      synchronized(lock){
           while(grill || griddle){
             //lock.wait();
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          griddle = true; 
          grill = true; 
        }
        cook(15);
        // synchronized(canCook){
        //   cook(15);
        // }
          synchronized(lock){
              griddle = false; 
              grill = false; 
              lock.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Orval is a chef needing 60 milliseconds to prepare a dish. */
  private static class Orval extends Chef {
    public void run() {
      while ( running ) {
      synchronized(lock){
           while( microwave || oven){
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          oven = true; 
          microwave = true; 
        }
        cook(60);
        // synchronized(canCook){
        //   cook(60);
        // }
          synchronized(lock){
              oven = false; 
              microwave = false; 
              lock.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Bridget is a chef needing 15 milliseconds to prepare a dish. */
  private static class Bridget extends Chef {
    public void run() {
      while ( running ) {
        synchronized(lock){
           while( microwave || fryer || grill){
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          fryer = true; 
          grill = true;
          microwave = true; 
        }
        cook(15);
        // synchronized(canCook){
        //   cook(15);
        // }
          synchronized(lock){
              fryer = false; 
              grill = false;
              microwave = false; 
              lock.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Lynda is a chef needing 45 milliseconds to prepare a dish. */
  private static class Lynda extends Chef {
    public void run() {
      while ( running ) {
      synchronized(lock){
           while( mixer || blender){
             try{
               lock.wait(); 
             }
             catch(InterruptedException e){
              System.out.println("Unable to lock\n");
             }
          }
          blender = true; 
          mixer = true; 
        }
        cook(45);
        // synchronized(canCook){
        //   cook(45);
        // }
          synchronized(lock){
              blender = false; 
              mixer = false; 
              lock.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  public static void main( String[] args ) throws InterruptedException {
    // Make a thread for each of our chefs.
    Chef chefList[] = {
      new Karon(),
      new Margret(),
      new Brigette(),
      new Elisabeth(),
      new Teri(),
      new Belinda(),
      new Eliza(),
      new Orval(),
      new Bridget(),
      new Lynda(),
    };

    // Start running all our chefs.
    for ( int i = 0; i < chefList.length; i++ )
      chefList[ i ].start();

    // Let the chefs cook for a while, then ask them to stop.
    Thread.sleep( 10000 );
    running = false;

    // Wait for all our chefs to finish, and collect up how much
    // cooking was done.
    int total = 0;
    for ( int i = 0; i < chefList.length; i++ ) {
      chefList[ i ].join();
      System.out.printf( "%s cooked %d dishes\n",
                         chefList[ i ].getClass().getSimpleName(),
                         chefList[ i ].dishCount );
      total += chefList[ i ].dishCount;
    }
    System.out.printf( "Total dishes cooked: %d\n", total );
  }
}
