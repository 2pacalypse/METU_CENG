import java.util.ArrayList;

public class CengTreeNodeInternal extends CengTreeNode
{
	private ArrayList<Integer> keys;
	private ArrayList<CengTreeNode> children;	
	
	public CengTreeNodeInternal(CengTreeNode parent) 
	{
		super(parent);
		
		// TODO: Extra initializations, if necessary.
		this.type = CengNodeType.Internal;
		keys = new ArrayList<Integer>();
		children = new ArrayList<CengTreeNode>();
	}
	
	// GUI Methods - Do not modify
	public ArrayList<CengTreeNode> getAllChildren()
	{
		return this.children;
	}
	public Integer keyCount()
	{
		return this.keys.size();
	}
	public Integer keyAtIndex(Integer index)
	{
		if(index >= this.keyCount() || index < 0)
		{
			return -1;
		}
		else
		{
			return this.keys.get(index);		
		}
	}
	
	// Extra Functions,
	
	public ArrayList<Integer> getKeys()
	{
		return this.keys;
	}
	
	public CengTreeNodeInternal(CengTreeNode parent,  ArrayList<Integer> a, ArrayList<CengTreeNode> b) 
	{
		super(parent);
		// TODO: Extra initializations
		this.type = CengNodeType.Internal;
		keys = a;
		children = b;
	}
	
	
	public int insertToInternal(int gkey){
		
			/*int i = 0;
			while(keys.size() > 0 && i < keys.size() &&  gkey > keyAtIndex(i)) {i++;}*/
		int l = 0;
		int h = keys.size()-1;
		while(l <= h){
			int m = (l+h)/2;
			if(gkey < keys.get(m).intValue()){
				h = m-1;
			}else if(gkey > keys.get(m).intValue()){
				l = m +1;
			}
		}
		
			keys.add(l, gkey);
			return l; 
		
				
	}
	
	
}
