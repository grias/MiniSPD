void USet()
{
    unordered_set<int> mySet {1, 2, 5, 3};

    for (auto &&item : mySet)
    {
        cout<<item<<endl;
    }
    
    if (mySet.contains(5)) // requires c++20 (gcc 8)
    {
        cout<<"Found 5 using contains"<<endl;
    }
    
    if (mySet.find(5) != mySet.end())
    {
        cout<<"Found 5 using find"<<endl;
    }

    if(mySet.count(5))
    {
        cout<<"Found 5 using count"<<endl;
    }

    for (auto it = mySet.begin(); it != mySet.end(); it++)
    {
        cout<<*it<<endl;
    }

    auto foundIt = find(mySet.begin(), mySet.end(), 2);
    if (foundIt != mySet.end())
    {
        int index = distance(mySet.begin(), foundIt);
        cout<<"Found 5 using algorithm::find at index "<<index<<endl;
    }

}