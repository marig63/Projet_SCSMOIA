package testJasper;

public class Coup {
	int depart;
	int arrivee;
	
	public Coup(int depart,int arrivee){
		this.depart = depart;
		this.arrivee = arrivee;
	}
	
	public String toString(){
		String res = "";
		
		res = "[" + depart +";" + arrivee +"]"; 
		
		return res;
	}
}
