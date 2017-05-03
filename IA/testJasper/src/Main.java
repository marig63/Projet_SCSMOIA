package preojet;
import se.sics.jasper.*;

public class Main {
    public static void main(String argv[]) {

        SICStus sp;
        SPTerm from, to, way;
        SPQuery query;
        int i;

        try
        {
            sp = new SICStus(argv,null);

            sp.load("/afs/deptinfo-st.univ-fcomte.fr/users/gmarisa/workspace/preojet/bin/preojet/test.pl");

            SPPredicate pred = new SPPredicate(sp, "pred", 5, "");
            SPTerm a = new SPTerm(sp, "text");
            SPTerm b = new SPTerm(sp, 42);
            SPTerm x = new SPTerm(sp).putVariable();
            SPTerm y = new SPTerm(sp).putVariable();
            SPTerm z = new SPTerm(sp).putVariable();

            query = sp.openQuery(pred, new SPTerm[] { a, b, x, y, z });

            while (query.nextSolution())
            {
                System.out.println(x.toString());
                System.out.println(y.toString());
                System.out.println(z.toString());
            }
        }
        catch ( Exception e )
        {
            e.printStackTrace();
        }
    }
}
