import java.util.*;
import java.io.*;
public class zerosum {
    private static int lengthArray; 
    private static boolean report; 
    private static int  workers; 
    private static List<Integer> array; 

    static class MyThread extends Thread {
        private int x;
        public int count;
        public MyThread( int x ) {
          this.x = x;
        }
        public void run() {
         int c = 0; 
          for ( int i = x; i < lengthArray; i+=workers ) {
            int sum = 0; 
            for(int j = i; j < lengthArray; j++){
              sum += array.get(j); 
              if(sum == 0){
                  c++; 
                  if(report)
                      System.out.println("" + i + " .. " + j); 
              }
            }
          }
          
          count = c; 
        }
      }

    public static void readFile(){
        Scanner s = new Scanner(System.in);
        while (s.hasNext()){
            //System.out.println("hi"); 
            array.add(Integer.parseInt(s.next()));
        }
        //System.out.println(array);
    }

    public static void main( String[] args ) {
        array = new ArrayList<>(); 
        workers = Integer.parseInt(args[0]); 
        String filepath;
        if(args.length > 1 && args[1].equals("report" )){
            report = true; 

        } 
        else{
            report = false; 
        } 
        readFile();
        int result = 0; 
        lengthArray = array.size();
        MyThread[] thread = new MyThread [ workers ];
        for ( int i = 0; i < workers; i++ ) {
          thread[ i ] = new MyThread( i );
          thread[ i ].start();
        }
        try {
          for ( int i = 0; i < thread.length; i++ ) {
            thread[ i ].join();
            result += thread[ i ].count ;          
          }
        } catch ( InterruptedException e ) {
          System.out.println( "Interrupted during join!" );
        }
        System.out.println("Total " + result) ;
      }
      
}