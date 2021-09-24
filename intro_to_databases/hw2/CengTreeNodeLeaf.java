import java.util.ArrayList;

public class CengTreeNodeLeaf extends CengTreeNode
{
	private ArrayList<CengGift> gifts;
	
	public CengTreeNodeLeaf(CengTreeNode parent) 
	{
		super(parent);
		// TODO: Extra initializations
		this.type = CengNodeType.Leaf;
		gifts = new ArrayList<CengGift>();
	}
	

	
	// GUI Methods - Do not modify
	public int giftCount()
	{
		return gifts.size();
	}
	public Integer giftKeyAtIndex(Integer index)
	{
		if(index >= this.giftCount())
		{
			return -1;
		}
		else
		{
			CengGift gift = this.gifts.get(index);
			
			return gift.key();
		}
	}
	
	// Extra Functions
	
	public void insertToLeaf(CengGift gift){
		/*int i = 0;
		while(gifts.size() > 0 && i < gifts.size() &&  key > giftKeyAtIndex(i)) {i++;}
		*/
		int l = 0;
		int h = giftCount()-1;
		int gkey = gift.key();
		while(l <= h){
			int m = (l+h)/2;
			if(gkey < gifts.get(m).key().intValue()){
				h = m-1;
			}else if(gkey > gifts.get(m).key().intValue()){
				l = m +1;
			}
		}
		gifts.add(l, gift);		
	}
	
	public ArrayList<CengGift> getGifts()
	{
		return this.gifts;
	}
	
	public CengTreeNodeLeaf(CengTreeNode parent,  ArrayList<CengGift> g ) 
	{
		super(parent);
		// TODO: Extra initializations
		this.type = CengNodeType.Leaf;
		gifts = g;
	}
	

}
