public class DiskSim{

  public static void main(String[] args){

    switch (args.length){
      case 1:
        if(args[0].equals("R")) System.out.println("Random Mode");
        break;
      case 3:
        if(args[2].charAt(0) == 'R') System.out.println("Random requests");
        break;
      default:
        System.out.println("Selected requests");
    }

  }


}
