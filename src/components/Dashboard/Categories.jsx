import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import Category from './Category'
import Stats from './Stats'
import { css } from 'glamor'

export default () => {
	useFirebaseConnect(['categories', 'fortunes'])
	const categories = useSelector(state => state.firebase.data.categories)
	const fortunes = useSelector(state => state.firebase.data.fortunes)

	const list = Object.keys(fortunes || {})
		.filter(f => fortunes[f].hasOwnProperty('category'))
		.map(f => ({ ...fortunes[f], key: f }))

	return list ? (
		<React.Fragment>
			<Stats fortunes={list} />
			<section {...css({ margin: '2rem 1rem' })}>
				{categories &&
					list &&
					categories.map((category, i) => {
						return (
							<Category
								key={category.id}
								{...category}
								fortunes={list}
								style={css({ marginTop: '2rem' })}
							/>
						)
					})}
			</section>
		</React.Fragment>
	) : null
}
