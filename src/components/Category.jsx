import React from 'react'

export default props => {
    return (
        <div className={'category ' + props.id}>
            <h3>{props.label}</h3>
        </div>
    )
}
