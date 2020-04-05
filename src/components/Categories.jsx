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

	return (
		<React.Fragment>
			<Stats fortunes={fortunes} />
			<section {...css({ margin: '2rem 1rem' })}>
				{categories &&
					categories.map((category, i) => {
						return (
							<Category
								key={category.id}
								{...category}
								fortunes={fortunes}
								style={css({ marginTop: '2rem' })}
							/>
						)
					})}
			</section>
		</React.Fragment>
	)
}
