/* \file
 * $Id$
 * \author Caton Little
 * \brief 
 *
 * \section LICENSE
 *
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is FieldML
 *
 * The Initial Developer of the Original Code is Auckland Uniservices Ltd,
 * Auckland, New Zealand. Portions created by the Initial Developer are
 * Copyright (C) 2010 the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 */

#ifndef H_FIELDML_STRUCTS
#define H_FIELDML_STRUCTS

#include <vector>
#include <set>
#include <string>

#include "fieldml_api.h"
#include "SimpleMap.h"
#include "SimpleBitset.h"

class FieldmlObject
{
public:
    const FieldmlHandleType type;
    const std::string name;
    
    //Virtual objects are either imports, or objects which are strict sub-objects (e.g. component ensembles, mesh element/chart arguments)/
    const bool isVirtual;

    int intValue;
    
    FieldmlObject( const std::string _name, FieldmlHandleType _type, bool _isVirtual );
    
    virtual ~FieldmlObject();
};


class EnsembleType :
    public FieldmlObject
{
public:
    const bool isComponentEnsemble;

    EnsembleMembersType type;
    
    FmlEnsembleValue min;
    
    FmlEnsembleValue max;
    
    int stride;
    
    int count;
    
    FmlObjectHandle dataSource;
    
    EnsembleType( const std::string _name, bool _isComponentEnsemble, bool _isVirtual );
};


class ElementSequence :
    public FieldmlObject
{
public:
    const FmlObjectHandle elementType;

    SimpleBitset members;
    
    ElementSequence( const std::string _name, FmlObjectHandle _componentType );
};


class BooleanType :
    public FieldmlObject
{
public:
    BooleanType( const std::string _name, bool _isVirtual );
};


class ContinuousType :
    public FieldmlObject
{
public:
    FmlObjectHandle componentType;
    
    ContinuousType( const std::string _name, bool _isVirtual );
};


class MeshType :
    public FieldmlObject
{
public:
    FmlObjectHandle chartType;
    FmlObjectHandle elementsType;
    
    SimpleMap<FmlEnsembleValue, std::string> shapes;
    
    MeshType( const std::string _name, bool _isVirtual );
};


class Evaluator :
    public FieldmlObject
{
public:
    const FmlObjectHandle valueType;

    Evaluator( const std::string _name, FieldmlHandleType _type, FmlObjectHandle _valueType, bool _isVirtual );
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates ) = 0;
};


class ReferenceEvaluator :
    public Evaluator
{
public:
    const FmlObjectHandle sourceEvaluator;

    SimpleMap<FmlObjectHandle, FmlObjectHandle> binds;

    ReferenceEvaluator( const std::string _name, FmlObjectHandle _evaluator, FmlObjectHandle _valueType, bool _isVirtual );
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );
};


class PiecewiseEvaluator :
    public Evaluator
{
public:
    FmlObjectHandle indexEvaluator;
    
    SimpleMap<FmlObjectHandle, FmlObjectHandle> binds;
    SimpleMap<FmlEnsembleValue, FmlObjectHandle> evaluators;
    
    PiecewiseEvaluator( const std::string name, FmlObjectHandle valueType, bool _isVirtual );
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );
};


class AggregateEvaluator :
    public Evaluator
{
public:
    SimpleMap<FmlObjectHandle, FmlObjectHandle> binds;
    SimpleMap<FmlEnsembleValue, FmlObjectHandle> evaluators;
    
    FmlObjectHandle indexEvaluator;
    
    AggregateEvaluator( const std::string _name, FmlObjectHandle _valueType, bool _isVirtual );
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );
};


class ArgumentEvaluator :
    public Evaluator
{
public:
    std::set<FmlObjectHandle> arguments;
    
    ArgumentEvaluator( const std::string name, FmlObjectHandle _valueType, bool _isVirtual );
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );
};


class ExternalEvaluator :
    public Evaluator
{
public:
    std::set<FmlObjectHandle> arguments;
    
    ExternalEvaluator( const std::string name, FmlObjectHandle _valueType, bool _isVirtual );
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );
};


class DataResource :
    public FieldmlObject
{
public:
    //NOTE: if isInline, this is the inline string. Otherwise, it's an href.
    std::string description;
    
    //NOTE: At the moment, inline resources may only be TEXT_PLAIN. 
    const std::string format;
    
    const DataResourceType type;

    std::vector<FmlObjectHandle> dataSources;
    
    DataResource( const std::string _name, DataResourceType _type, const std::string _format, const std::string _description );
        
    virtual ~DataResource();
};

    
class DataSource :
    public FieldmlObject
{
protected:
    DataSource( const std::string _name, DataResource *_resource, DataSourceType _type );
    
public:
    const DataSourceType type;
    
    DataResource * const resource;
    
    virtual ~DataSource()
    {
    }
};

    
class ArrayDataSource :
    public DataSource
{
public:
    //NOTE: For text resources, this is a line number. For HDF5 files, this is a dataset name.
    const std::string location;
    
    const int rank;
    
    std::vector<int> offsets;
    
    std::vector<int> sizes;
    
    //NOTE: Optional for formats that internally specify sizes.
    std::vector<int> rawSizes;
    
    ArrayDataSource( const std::string _name, DataResource *_resource, const std::string _location, int _rank );
    
    virtual ~ArrayDataSource();
};


class BaseDataDescription
{
public:
    const DataDescriptionType descriptionType;
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates ) = 0;

    virtual FmlErrorNumber addIndexEvaluator( bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator ) = 0;
    
    virtual FmlErrorNumber setIndexEvaluator( int index, bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator ) = 0;
    
    virtual FmlErrorNumber getIndexEvaluator( int index, bool isSparse, FmlObjectHandle &evaluator ) = 0;
    
    virtual FmlErrorNumber setIndexEvaluator( int index, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator ) = 0;
    
    virtual FmlErrorNumber getIndexEvaluator( int index, FmlObjectHandle &evaluator ) = 0;
    
    virtual FmlErrorNumber getIndexOrder( int index, FmlObjectHandle &order ) = 0;
    
    virtual int getIndexCount( bool isSparse ) = 0;

    virtual ~BaseDataDescription() = 0;
    
protected:
    BaseDataDescription( DataDescriptionType _descriptionType );
};


class UnknownDataDescription :
    public BaseDataDescription
{
public:
    UnknownDataDescription();

    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );

    virtual FmlErrorNumber addIndexEvaluator( bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber setIndexEvaluator( int index, bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber getIndexEvaluator( int index, bool isSparse, FmlObjectHandle &evaluator );
    
    virtual FmlErrorNumber setIndexEvaluator( int index, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber getIndexEvaluator( int index, FmlObjectHandle &evaluator );
    
    virtual FmlErrorNumber getIndexOrder( int index, FmlObjectHandle &order );
    
    virtual int getIndexCount( bool isSparse );

    virtual ~UnknownDataDescription();
    
protected:
    UnknownDataDescription( DataDescriptionType _descriptionType );
};


class DenseArrayDataDescription :
    public BaseDataDescription
{
private:
    std::vector<FmlObjectHandle> denseIndexes;
    std::vector<FmlObjectHandle> denseOrders;
        
public:
    FmlObjectHandle dataSource;
    
    DenseArrayDataDescription();

    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );

    virtual FmlErrorNumber addIndexEvaluator( bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber setIndexEvaluator( int index, bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber getIndexEvaluator( int index, bool isSparse, FmlObjectHandle &evaluator );
    
    virtual FmlErrorNumber setIndexEvaluator( int index, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber getIndexEvaluator( int index, FmlObjectHandle &evaluator );
    
    virtual FmlErrorNumber getIndexOrder( int index, FmlObjectHandle &order );
    
    virtual int getIndexCount( bool isSparse );
    
    virtual ~DenseArrayDataDescription();
};


class DokArrayDataDescription :
    public BaseDataDescription
{
private:
    std::vector<FmlObjectHandle> sparseIndexes;
    std::vector<FmlObjectHandle> denseIndexes;
    std::vector<FmlObjectHandle> denseOrders;
        
public:
    FmlObjectHandle keySource;
    FmlObjectHandle valueSource;
    
    DokArrayDataDescription();

    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );

    virtual FmlErrorNumber addIndexEvaluator( bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber setIndexEvaluator( int index, bool isSparse, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber getIndexEvaluator( int index, bool isSparse, FmlObjectHandle &evaluator );
    
    virtual FmlErrorNumber setIndexEvaluator( int index, FmlObjectHandle evaluator, FmlObjectHandle orderEvaluator );
    
    virtual FmlErrorNumber getIndexEvaluator( int index, FmlObjectHandle &evaluator );
    
    virtual FmlErrorNumber getIndexOrder( int index, FmlObjectHandle &order );
    
    virtual int getIndexCount( bool isSparse );
    
    virtual ~DokArrayDataDescription();
};


class ParameterEvaluator :
    public Evaluator
{
public:
    BaseDataDescription *dataDescription;
    
    ParameterEvaluator( const std::string _name, FmlObjectHandle _valueType, bool _isVirtual );
    
    virtual void addDelegates( std::set<FmlObjectHandle> &delegates );
    
    virtual ~ParameterEvaluator();
};


#endif //H_FIELDML_STRUCTS
